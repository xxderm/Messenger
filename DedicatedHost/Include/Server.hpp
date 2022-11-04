#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"

#pragma comment(lib, "Ws2_32.lib")

namespace DedicatedHost {

    class Server final {
    public:
        bool Initialize(const char* port);

        void Launch() noexcept;

        void Stop() noexcept;

        ~Server();
    private:
    private:
        bool mRunning = false;
        WSAData mWSAData;
        SOCKET mServerSocket = INVALID_SOCKET;
    };

}