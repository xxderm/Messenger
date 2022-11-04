#include "Connection.hpp"

namespace Application {

    bool TcpCoonection::Initialize(uint32_t port) noexcept {
        if (SDLNet_Init() < 0) {
            std::cout << "Couldn't init sdlnet\n";
            SDL_Quit();
        }
        SDLNet_ResolveHost(&mIp, "127.0.0.1", port);
        mSocket = SDLNet_TCP_Open(&mIp);
    }

    TcpCoonection::~TcpCoonection() {

    }

    bool TcpCoonection::Send(std::shared_ptr<Utils::IPacket> packet) {
        char* hello = "HUILOY\n";
        SDLNet_TCP_Send(mSocket, hello, strlen(hello));
    }

}