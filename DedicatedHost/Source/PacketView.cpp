#include "PacketView.hpp"

namespace DedicatedHost::View {

    void PacketView::Render(bool &running) {
        while (SDL_PollEvent(&mEvent)) {
            ImGui_ImplSDL2_ProcessEvent(&mEvent);
            if (mEvent.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mWindow);
        ImGui::NewFrame();

        glClearColor(0.17, 0.18, 0.24, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 200));
        ImGui::Begin("##PacketStreamWindor", 0, ImGuiWindowFlags_NoDecoration);
        if (ImGui::BeginTable("Packet stream", 2)) {
            ImGui::TableNextRow();

            for (auto& inPack : mInputPackets) {
                ImGui::TableSetColumnIndex(0);
                ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                       IM_COL32(38, 42, 103, 255));
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                std::string str = "\n\n\n" + inPack + "\n\n\n";
                ImGui::Text(str.c_str());
            }

            for (auto& outPack : mOutputPackets) {
                ImGui::TableSetColumnIndex(1);
                ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                       IM_COL32(38, 42, 103, 255));
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                std::string str = "\n\n\n" + outPack + "\n\n\n";
                ImGui::Text(str.c_str());
            }

            ImGui::TableNextRow();


            ImGui::EndTable();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(mWindow);
    }

    void PacketView::AddOutPacket(Utils::Signal signal) {
        std::string result = DetectSignalName(signal);
        if (mOutputPackets.size() >= 100)
            mOutputPackets.clear();
        mOutputPackets.push_back(result);
    }

    void PacketView::AddInPacket(Utils::Signal signal) {
        std::string result = DetectSignalName(signal);
        if (mInputPackets.size() >= 100)
            mInputPackets.clear();
        mInputPackets.push_back(result);
    }

    void PacketView::Initialize() {
        mWindow = SDL_CreateWindow("Dedicated Host",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   400, 200, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        mContext = SDL_GL_CreateContext(mWindow);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        auto& io = ImGui::GetIO();
        (void)io;

        ImGui::StyleColorsDark();

        mRenderer = SDL_CreateRenderer(mWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
        ImGui_ImplOpenGL3_Init();

        SDL_GL_SetSwapInterval(1);
    }

    std::string PacketView::DetectSignalName(Utils::Signal signal) {
        switch (signal) {
            case Utils::CONNECT:
                return "ConnectPacket";
                break;
            case Utils::GUESTS:
                return "GuestsPacket";
                break;
            case Utils::CHANNELS:
                return "ChannelsPacket";
                break;
            case Utils::NEW_CHANNEL:
                return "NewChannelPacket";
                break;
            case Utils::CLOSE_CHANNEL:
                return "CloseChannelPacket";
                break;
            case Utils::GUEST_INFO_UPDATE:
                return "GuestInfoUpdatePacket";
                break;
            case Utils::PERSONAL_MESSAGE:
                return "PersonalMessagePacket";
                break;
            case Utils::CONNECT_CHANNEL:
                return "ConnectChannelPacket";
                break;
            case Utils::DISCONNECT:
                return "UserDisconnectPacket";
                break;
            case Utils::CHANNEL_LEAVE:
                return "ChannelLeavePacket";
                break;
            case Utils::CHANNEL_MESSAGE:
                return "ChannelMessagePacket";
                break;
            case Utils::SERVER_STOP:
                return "ServerStopPacket";
                break;
            case Utils::CHANNEL_INFO_UPDATE:
                return "ChannelInfoUpdatePacket";
                break;
        }
        return std::string("undefined packet");
    }
}