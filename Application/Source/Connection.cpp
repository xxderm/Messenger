#include "Connection.hpp"

namespace Application::Model {
    TcpConnection::~TcpConnection() {
        SDLNet_TCP_Close(mSocket);
    }

    bool TcpConnection::Send(char* data, uint32_t len) {
        if (SDLNet_TCP_Send(mSocket, data, len) >= 0)
            return true;
        return false;
    }

    bool TcpConnection::Connect() {
        mSocket = SDLNet_TCP_Open(&mServerIp);
        if (!mSocket)
            return false;
        return true;
    }

    bool TcpConnection::Init(uint32_t port) {
        this->mServerPort = port;
        //SDL_Init(SDL_INIT_EVERYTHING);
        if (SDLNet_ResolveHost(&mServerIp, "127.0.0.1", port) == -1)
            return false;
        return true;
    }

    void TcpConnection::Disconnect() {
        SDLNet_TCP_Close(mSocket);
    }

    bool TcpConnection::Receive(std::pair<Utils::Signal, std::shared_ptr<Utils::InputMemory>>& signalData) {
        char buffer[DEFAULT_BUFFER_LEN];
        int countRec = SDLNet_TCP_Recv(mSocket, buffer, DEFAULT_BUFFER_LEN);
        if (countRec > 0) {
            signalData = Utils::SignalManager::GetSignal(buffer);
            return true;
        }
        return false;
    }

    TcpConnection::TcpConnection() {
        SDLNet_Init();
    }

}