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
#include "Entities.hpp"

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

        void RequestUpdate();

        void Request(Utils::Signal signal);

        void GuestsModelUpdate(std::shared_ptr<Utils::InputMemory> input);

        void ChannelsModelUpdate(std::shared_ptr<Utils::InputMemory> input);

        void SendConnectInfo();

        void SendGuestInfo();

        void SendDisconnect();

        void ConnectUID(std::shared_ptr<Utils::InputMemory> input);

        void ChannelCreate(std::string title, uint32_t places, bool access);

        void JoinChannel(uint32_t uid, std::string channelTitle);

        void UpdateChannelInfo(std::shared_ptr<Utils::InputMemory> input);

        void OnChannelMessageSend(uint32_t id, std::string title, std::string msg);
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
        std::shared_ptr<Utils::BackgroundWorker> mRequestWorker;
        std::shared_ptr<std::thread> mListenThread;
        Model::Guests mGuests;
        Model::Messages mMessages;
        Model::Channels mChannels;
        std::shared_ptr<std::string> mUserName;
        std::shared_ptr<uint32_t> mUid;
    };

}