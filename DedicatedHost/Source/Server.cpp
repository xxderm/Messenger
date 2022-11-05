#include "../Include/Server.hpp"

namespace DedicatedHost {

    bool Server::Initialize(uint32_t port) {
        SDL_Init(SDL_INIT_EVERYTHING);
        if (SDLNet_Init() < 0) {
            std::cout << "Couldn't init sdlnet\n";
            SDL_Quit();
        }
        if (SDLNet_ResolveHost(&mIp, NULL, port) == -1) {
            std::cout << "Resolve Error\n";
            SDL_Quit();
        }
        mServerSocket = SDLNet_TCP_Open(&mIp);
        if (mServerSocket == 0) {
            std::cout << "Socket open error\n";
            SDL_Quit();
        }
        mSockSet = SDLNet_AllocSocketSet(MaxGuests);
        if (mSockSet == 0) {
            std::cout << "SockSet error\n";
            SDL_Quit();
        }
        SDLNet_TCP_AddSocket(mSockSet, mServerSocket);
        return true;
    }

    void Server::Launch() noexcept {
        mRunning = true;
        int index = 0;
        while (mRunning) {
            auto countRdy = SDLNet_CheckSockets(mSockSet, 10);
            if (countRdy == -1) std::cout << SDLNet_GetError();
            if (countRdy > 0) {
                if (SDLNet_SocketReady(mServerSocket)) {
                    int getSock = AcceptSock(index);
                    if (!getSock) {
                        --countRdy;
                        continue;
                    }
                    int checkCount;
                    for (checkCount = 0; checkCount < MaxGuests; ++checkCount) {
                        if (mGuests[(index + checkCount) % MaxGuests] == 0)
                            break;
                    }
                    index = (index + checkCount) % MaxGuests;
                    --countRdy;
                }
                for (int i = 0; (i < MaxGuests) && countRdy ; ++i) {
                    if (mGuests[i] == 0) continue;
                    if (!SDLNet_SocketReady(mGuests[i])) continue;
                    auto data = Receive(i);
                    if (data == 0) {
                        --countRdy;
                        continue;
                    }
                    std::cout << data << std::endl;
                    delete data;
                    --countRdy;
                }
            }
        }
    }

    Server::~Server() {
        if (SDLNet_TCP_DelSocket(mSockSet, mServerSocket) == -1) {
            std::cout << "Failed to delete server socket\n";
        }
        SDLNet_FreeSocketSet(mSockSet);
        SDLNet_TCP_Close(mServerSocket);
    }

    void Server::Stop() noexcept {
        mRunning = false;
    }

    int Server::AcceptSock(int index) {
        if (mGuests[index]) {
            std::cout << "Socket override\n";
            CloseSock(index);
        }
        mGuests[index] = SDLNet_TCP_Accept(mServerSocket);
        if (mGuests[index] == 0)
            return 0;
        if (SDLNet_TCP_AddSocket(mSockSet, mGuests[index]) == -1) {
            std::cout << "Error add socket\n";
            SDL_Quit();
        }
        std::cout << "New client: " << mGuests[index] << std::endl;
        return 1;
    }

    void Server::CloseSock(int index) {
        if (mGuests[index] == 0) {
            std::cout << "Error add socket\n";
            return;
        }
        if (SDLNet_TCP_DelSocket(mSockSet, mGuests[index]) == -1) {
            std::cout << "Error delete socket\n";
            SDL_Quit();
        }
        SDLNet_TCP_Close(mGuests[index]);
        mGuests[index] = 0;
    }

    char* Server::Receive(int index) {
        char* buffer[DEFAULT_BUFFER_LEN];
        int countRec = SDLNet_TCP_Recv(mGuests[index], buffer, DEFAULT_BUFFER_LEN);
        if (countRec <= 0) {
            std::cout << "Client disconnect: " << mGuests[index] << std::endl;
            CloseSock(index);
            return 0;
        }
        else {
            char* data = new char[DEFAULT_BUFFER_LEN];
            memcpy(data, buffer, DEFAULT_BUFFER_LEN);
            return data;
        }
    }

}