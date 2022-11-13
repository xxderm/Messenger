#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include <SDL_net.h>
#include <list>
#include <functional>
#include "Packet.hpp"

namespace DedicatedHost::Model {

    constexpr uint32_t MaxGuests = 64;

    class Server final {
    public:
        bool Initialize(uint32_t port);

        void Launch() noexcept;

        void Stop() noexcept;

        void OnReceive(std::function<void(Utils::Signal, std::shared_ptr<Utils::InputMemory>)> fn);

        void OnConnect(std::function<void(uint32_t)> fn);

        void OnDisconnect(std::function<void(uint32_t)> fn);

        bool Send(std::shared_ptr<Utils::Packet> packet);

        bool SendTo(uint32_t index, std::shared_ptr<Utils::Packet> packet);

        ~Server();
    private:
        int AcceptSock(int index);
        void CloseSock(int index);
        char* Receive(int index);
        void ProcessPacket(char* data);
    private:
        bool mRunning = false;
        TCPsocket mServerSocket;
        SDLNet_SocketSet mSockSet;
        TCPsocket mGuests[MaxGuests];
        IPaddress mIp;
        std::function<void(Utils::Signal, std::shared_ptr<Utils::InputMemory>)> mReceiveCallBack;
        std::function<void(uint32_t)> mConnectCallBack;
        std::function<void(uint32_t)> mDisconnectCallBack;
    };

}