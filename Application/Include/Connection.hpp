#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"
#include <SDL.h>
#include <SDL_net.h>

namespace Application {

    class TcpCoonection final {
    public:
        bool Initialize(uint32_t port) noexcept;

        bool Send(std::shared_ptr<Utils::IPacket> packet);

        ~TcpCoonection();
    private:
        TCPsocket mSocket;
        IPaddress mIp;
    };

}