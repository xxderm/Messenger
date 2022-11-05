#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"
#include <SDL_net.h>
#include <SDL.h>

namespace Application {

    class TcpCoonection final {
    public:
        TcpCoonection();

        bool Initialize(uint32_t port);

        bool Connect(uint32_t port);

        bool Send();

        ~TcpCoonection();
    private:
        TCPsocket mSocket;
        IPaddress mServerIp;
    };

}