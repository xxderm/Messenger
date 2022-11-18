#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "MainView.hpp"
#include "Connection.hpp"
#include "BackgroundWorker.hpp"
#include <thread>

namespace Application::Controller {

    class ClientController final {
    public:
        ClientController() = default;

        void Initialize();

        void Render();

        ~ClientController();
    private:
        void ServerConnection();

        void HandleReceive();
    private:
        SDL_Renderer* mRenderer{};
        SDL_GLContext mContext{};
        SDL_Window* mWindow{};
        SDL_Event mEvent{};
        bool mRun = false;
        bool mServerStatus{};
        std::string mStatusConnection{};
        std::shared_ptr<Model::TcpConnection> mConnection;
        std::shared_ptr<View::MainView> mMainView;
        std::shared_ptr<Utils::BackgroundWorker> mConnectRetry;
        std::shared_ptr<std::thread> mListenThread;
    };

}