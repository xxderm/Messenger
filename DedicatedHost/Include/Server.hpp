#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"
#include <SDL_net.h>
#include <list>

namespace DedicatedHost {

    constexpr uint32_t MaxGuests = 64;

    class Server final {
    public:
        bool Initialize(uint32_t port);

        void Launch() noexcept;

        void Stop() noexcept;

        ~Server();
    private:
        int AcceptSock(int index);
        void CloseSock(int index);
        char* Receive(int index);
    private:
        bool mRunning = false;
        TCPsocket mServerSocket;
        SDLNet_SocketSet mSockSet;
        TCPsocket mGuests[MaxGuests];
        IPaddress mIp;
    };

}