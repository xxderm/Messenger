#include "Connection.hpp"

namespace Application {
    TcpCoonection::~TcpCoonection() {
        SDLNet_TCP_Close(mSocket);
    }

    bool TcpCoonection::Send(void* data, uint32_t len) {
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

}