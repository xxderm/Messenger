#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"
#include <SDL_net.h>
#include <SDL.h>

namespace Application::Model {

    class TcpCoonection final {
    public:
        TcpCoonection() = default;

        bool Init(uint32_t port);

        bool Connect();

        void Disconnect();

        bool Send(char* data, uint32_t len);

        bool Receive();

        ~TcpCoonection();
    private:
        TCPsocket mSocket;
        IPaddress mServerIp;
        uint32_t mServerPort;
    };

}