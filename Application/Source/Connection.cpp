#include "Connection.hpp"

namespace Application {
    bool TcpCoonection::Initialize(uint32_t port) {
        SDL_Init(SDL_INIT_EVERYTHING);
        if (SDLNet_Init() < 0) {
            std::cout << "Couldn't init sdlnet\n";
            SDL_Quit();
        }
        if (SDLNet_ResolveHost(&mServerIp, "127.0.0.1", port) == -1) {
            std::cout << "Resolve Error\n";
            SDL_Quit();
        }
        mSocket = SDLNet_TCP_Open(&mServerIp);
        if (!mSocket) {
            std::cout << "Socket open error\n";
            SDL_Quit();
        }
        return true;
    }

    TcpCoonection::~TcpCoonection() {

    }

    bool TcpCoonection::Send() {
        char* hello = "HUILOY\n\r";
        SDLNet_TCP_Send(mSocket, hello, strlen(hello));
        return true;
    }

    TcpCoonection::TcpCoonection() {

    }

    bool TcpCoonection::Connect() {
        return false;
    }

}