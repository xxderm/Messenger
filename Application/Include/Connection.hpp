#pragma once
#include <iostream>
#include <sstream>
#include <thread>
#include "Packet.hpp"
#include <SDL_net.h>
#include <SDL.h>
#include <functional>

namespace Application::Model {

    class TcpConnection final {
    public:
        TcpConnection();

        bool Init(uint32_t port);

        bool Connect();

        void Disconnect();

        bool Send(char* data, uint32_t len);

        bool Receive(std::pair<Utils::Signal, std::shared_ptr<Utils::InputMemory>>& signalData);

        ~TcpConnection();
    private:
        TCPsocket mSocket;
        IPaddress mServerIp;
        uint32_t mServerPort;
    };

}