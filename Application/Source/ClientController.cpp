#include "ClientController.hpp"

namespace Application::Controller {

    static SDL_HitTestResult SDLCALL HitTest(SDL_Window *window, const SDL_Point *pt, void *data) {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        const SDL_Rect dragArea[] =  {
                { 0, 0, w - 50, 20}
        };
        for (const auto & i : dragArea) {
            if (SDL_PointInRect(pt, &i))
                return SDL_HITTEST_DRAGGABLE;
        }
        return SDL_HITTEST_NORMAL;
    }

    void ClientController::Initialize() {
        mMainView = std::make_shared<View::MainView>();
        mConnectRetry = std::make_shared<Utils::BackgroundWorker>();
        mRequestWorker = std::make_shared<Utils::BackgroundWorker>();
        mConnection = std::make_shared<Model::TcpConnection>();
        mUserName = std::make_shared<std::string>("UserName");
        mUid = std::make_shared<uint32_t>(0);
        mMainView->UpdateUserName(mUserName);
        mMainView->SetUid(mUid);
        mMainView->OnUserNameUpdate([this](){ this->SendGuestInfo(); });
        mMainView->OnJoinChannel([this](uint32_t uid, std::string title){ this->JoinChannel(uid, title); });
        mMainView->OnSendMessageChannel([this](uint32_t uid, std::string title, std::string msg){
           this->OnChannelMessageSend(uid, title, msg);
        });
        mMainView->OnChannelCreate([this](std::string _title, uint32_t _places, bool _access){
            this->ChannelCreate(_title, _places, _access);
        });
        mConnection->Init(5000);
        mServerStatus = mConnection->Connect();

        mConnectRetry->SetInterval(3000);
        mConnectRetry->OnTick([this]() {
            this->ServerConnection();
        });

        mRequestWorker->SetInterval(1000);
        mRequestWorker->OnTick([this](){
                this->RequestUpdate();
        });

        SDL_Init(SDL_INIT_EVERYTHING);
        mWindow = SDL_CreateWindow("Messenger",
                                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
        mContext = SDL_GL_CreateContext(mWindow);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        (void)io;
        mMainView->Initialize();
        mMainView->SetRunningPtr(&mRun);
        io.Fonts->Fonts[0];

        ImGui::StyleColorsDark();

        mRenderer = SDL_CreateRenderer(mWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        SDL_SetWindowHitTest(mWindow, HitTest, 0);

        ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
        ImGui_ImplOpenGL3_Init();

        SDL_GL_SetSwapInterval(1);
        mRun = true;
        mListenThread = std::make_shared<std::thread>([&](){
            while (mRun) {
                if (!mServerStatus)
                    continue;
                this->HandleReceive();
            }
        });
        mConnectRetry->Start();
        mRequestWorker->Start();
        mListenThread->detach();
        if (mServerStatus)
            SendConnectInfo();
    }

    void ClientController::Render() {
        while (mRun) {
            while (SDL_PollEvent(&mEvent)) {
                ImGui_ImplSDL2_ProcessEvent(&mEvent);
                if (mEvent.type == SDL_QUIT)
                    mRun = false;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(mWindow);
            ImGui::NewFrame();

            glClearColor(0.17, 0.18, 0.24, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render
            mMainView->SetServerStatus(mServerStatus);
            if (!mServerStatus)
                mStatusConnection = "повторное подключение через " +
                                    std::to_string(mConnectRetry->GetElapsedSec()) + " секунд...";
            mMainView->SetStatusContent(mStatusConnection);
            mMainView->Render(this->mWindow, mRenderer);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(mWindow);
        }
    }

    ClientController::~ClientController() {
        this->SendDisconnect();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    void ClientController::ServerConnection() {
        if (!mServerStatus) {
            mStatusConnection = "";
            mConnection->Init(5000);
            if (mConnection->Connect()) {
                mServerStatus = true;
                SendConnectInfo();
            }
        }
    }

    void ClientController::HandleReceive() {
        std::pair<Utils::Signal, std::shared_ptr<Utils::InputMemory>> signalData;
        if (mConnection->Receive(signalData)) {
            switch (signalData.first) {
                case Utils::CONNECT:
                    ConnectUID(signalData.second);
                    Request(Utils::Signal::GUESTS);
                    Request(Utils::Signal::CHANNELS);
                    break;
                case Utils::GUESTS:
                    GuestsModelUpdate(signalData.second);
                    break;
                case Utils::CHANNELS:
                    ChannelsModelUpdate(signalData.second);
                    break;
                case Utils::CHANNEL_INFO_UPDATE:
                    UpdateChannelInfo(signalData.second);
                    break;
                case Utils::NEW_CHANNEL:
                    Request(Utils::Signal::GUESTS);
                    Request(Utils::Signal::CHANNELS);
                    Request(Utils::Signal::CHANNEL_INFO_UPDATE);
                    break;
                case Utils::CLOSE_CHANNEL:
                    Request(Utils::Signal::GUESTS);
                    Request(Utils::Signal::CHANNELS);
                    break;
                case Utils::GUEST_INFO_UPDATE:
                    break;
                case Utils::PERSONAL_MESSAGE:
                    break;
                case Utils::CONNECT_CHANNEL:
                    break;
                case Utils::DISCONNECT:
                    break;
                case Utils::CHANNEL_LEAVE:
                    break;
                case Utils::CHANNEL_MESSAGE:
                    break;
                case Utils::SERVER_STOP:
                    mServerStatus = false;
                    break;
            }
        }
    }

    void ClientController::RequestUpdate() {
        if (mServerStatus) {
            // TODO:
            //Request(Utils::Signal::GUESTS);
            //Request(Utils::Signal::CHANNELS);
            //Request(Utils::Signal::CHANNEL_INFO_UPDATE);
        }
    }

    void ClientController::Request(Utils::Signal signal) {
        Utils::PacketRequest request(signal);
        auto data = request.Data();
        auto len = request.OutputDataSize();
        mConnection->Send(data, len);
    }

    void ClientController::GuestsModelUpdate(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::GuestsPacket(input);
        mGuests.clear();
        auto guests = packet.GetGuests();
        for (auto& guest : guests)
            mGuests.push_back(Model::Guest(guest));
        mMainView->UpdateGuestsModel(mGuests);
    }

    void ClientController::ChannelsModelUpdate(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ChannelsPacket(input);
        mChannels.clear();
        auto channels = packet.GetChannels();
        for (auto& channel : channels)
            mChannels.push_back(Model::Channel(channel));
        mMainView->UpdateChannelsModel(mChannels);
        Request(Utils::Signal::CHANNEL_INFO_UPDATE);
    }

    void ClientController::SendConnectInfo() {
        auto packetRequest = Utils::ConnectPacket(*mUserName.get());
        auto data = packetRequest.Data();
        auto len = packetRequest.OutputDataSize();
        mConnection->Send(data, len);
    }

    void ClientController::SendGuestInfo() {
        auto packetRequest = Utils::GuestUpdatePacket(*mUserName.get(), *mUid.get());
        auto data = packetRequest.Data();
        auto len = packetRequest.OutputDataSize();
        mConnection->Send(data, len);
    }

    void ClientController::ConnectUID(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ConnectPacket(input);
        auto name = packet.GetName();
        auto id = packet.GetId();
        std::cout << "ID:" << id << std::endl;
        *mUserName = name + "#" + std::to_string(id);
        *mUid.get() = id;
    }

    void ClientController::SendDisconnect() {
        auto packet = Utils::DisconnectPacket(*mUid.get());
        auto data = packet.Data();
        auto len = packet.OutputDataSize();
        mConnection->Send(data, len);
    }

    void ClientController::ChannelCreate(std::string title, uint32_t places, bool access) {
        Utils::ChannelPacketProperties prop;
        prop.Name = title;
        prop.Places = places;
        prop.Access = access;
        auto packet = Utils::NewChannelPacket(std::make_shared<Utils::ChannelPacketProperties>(prop), *mUid.get());
        auto data = packet.Data();
        auto len = packet.OutputDataSize();
        mConnection->Send(data, len);
        JoinChannel(*mUid.get(), prop.Name);
        Request(Utils::Signal::CHANNELS);
        Request(Utils::Signal::CHANNEL_INFO_UPDATE);
    }

    void ClientController::JoinChannel(uint32_t uid, std::string channelTitle) {
        auto packet = Utils::ChannelConnectPacket(uid, channelTitle);
        auto data = packet.Data();
        auto len = packet.OutputDataSize();
        mConnection->Send(data, len);
        Request(Utils::Signal::CHANNELS);
        Request(Utils::Signal::CHANNEL_INFO_UPDATE);
    }

    void ClientController::UpdateChannelInfo(std::shared_ptr<Utils::InputMemory> input) {
        if(mChannels.empty()) return;
        auto packet = Utils::ChannelInfoUpdatePacket(input);
        auto channelInfo = packet.GetChannel();
        for (auto& channel : mChannels) {
            if (channel.Title == channelInfo->Title) {
                channel.Users.clear();
                channel.UsrMessages.clear();
                for (auto& usrInfo : channelInfo->Users) {
                    auto id = usrInfo.Id;
                    auto name = usrInfo.Name;
                    auto guestProp = std::make_shared<Utils::GuestProperties>();
                    guestProp->Name = name;
                    guestProp->Id = id;
                    channel.Users.push_back(guestProp);
                }
                for (auto& msgInfo : channelInfo->Messages) {
                    auto uid = msgInfo.Id;
                    auto content = msgInfo.Message;
                    Model::Message msg;
                    msg.Content = content;
                    msg.User = mGuests[uid];
                    channel.UsrMessages.push_back(msg);
                }
                break;
            }
        }
        mMainView->UpdateChannelsModel(mChannels);
    }

    void ClientController::OnChannelMessageSend(uint32_t id, std::string title, std::string msg) {
        std::cout << "ID: " << id << ", Channel: " << title << ", Message: " << msg << std::endl;
        auto packet = Utils::ChannelMessagePacket(id, title, msg);
        auto data = packet.Data();
        auto len = packet.OutputDataSize();
        mConnection->Send(data, len);
        Request(Utils::Signal::CHANNEL_INFO_UPDATE);
    }

}