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
        mConnection = std::make_shared<Model::TcpConnection>();
        mConnection->Init(5000);
        mServerStatus = mConnection->Connect();

        mConnectRetry->SetInterval(3000);
        mConnectRetry->OnTick([this]() {
            this->ServerConnection();
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
        mListenThread->detach();
        mConnectRetry->Start();
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
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    void ClientController::ServerConnection() {
        std::cout << "TIck\n";
        if (!mServerStatus) {
            std::cout << "Try\n";
            mStatusConnection = "";
            mConnection->Init(5000);
            mServerStatus = mConnection->Connect();
        }
        else {
            Utils::PacketRequest request(Utils::Signal::GUESTS);
            auto data = request.Data();
            auto len = request.OutputDataSize();
            mConnection->Send(data, len);
        }
    }

    void ClientController::HandleReceive() {
        std::pair<Utils::Signal, std::shared_ptr<Utils::InputMemory>> signalData;
        if (mConnection->Receive(signalData)) {
            std::cout << signalData.first << std::endl;
            if (signalData.first == Utils::Signal::SERVER_STOP)
                mServerStatus = false;
        }
    }

}