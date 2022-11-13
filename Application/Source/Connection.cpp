#include "Connection.hpp"

namespace Application::Model {
    TcpCoonection::~TcpCoonection() {
        SDLNet_TCP_Close(mSocket);
    }

    bool TcpCoonection::Send(char* data, uint32_t len) {
        if (SDLNet_TCP_Send(mSocket, data, len) >= 0)
            return true;
        return false;
    }

    bool TcpCoonection::Connect() {
        mSocket = SDLNet_TCP_Open(&mServerIp);
        if (!mSocket)
            return false;
        return true;
    }

    bool TcpCoonection::Init(uint32_t port) {
        this->mServerPort = port;
        SDL_Init(SDL_INIT_EVERYTHING);
        if (SDLNet_Init() < 0)
            return false;
        if (SDLNet_ResolveHost(&mServerIp, "127.0.0.1", port) == -1)
            return false;
        return true;
    }

    void TcpCoonection::Disconnect() {
        SDLNet_TCP_Close(mSocket);
    }

    bool TcpCoonection::Receive() {
        char buffer[DEFAULT_BUFFER_LEN];
        int countRec = SDLNet_TCP_Recv(mSocket, buffer, DEFAULT_BUFFER_LEN);
        if (countRec > 0) {
            return true;
        }
        return false;
    }

}