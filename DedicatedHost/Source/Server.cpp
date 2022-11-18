#include <Packet.hpp>
#include "../Include/Server.hpp"

namespace DedicatedHost::Model {

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
                    ProcessPacket(data);
                    delete data;
                    --countRdy;
                }
            }
        }
    }

    Server::~Server() {
        Stop();
        if (SDLNet_TCP_DelSocket(mSockSet, mServerSocket) == -1) {
            std::cout << "Failed to delete server socket\n";
        }
        SDLNet_FreeSocketSet(mSockSet);
        SDLNet_TCP_Close(mServerSocket);
    }

    void Server::Stop() noexcept {
        std::shared_ptr<Utils::Packet> response =
                std::make_shared<Utils::PacketRequest>(Utils::PacketRequest(Utils::Signal::SERVER_STOP));
        this->Send(response);
        std::cout << "Send STOP\n";
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
        mConnectCallBack(index);
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
            mDisconnectCallBack(index);
            return 0;
        }
        else {
            char* data = new char[DEFAULT_BUFFER_LEN];
            memcpy(data, buffer, DEFAULT_BUFFER_LEN);
            return data;
        }
    }

    void Server::ProcessPacket(char* data) {
        try {
            auto signal = Utils::SignalManager::GetSignal(data);
            std::cout << signal.first << std::endl;
            mReceiveCallBack(signal.first, signal.second);
        }
        catch (...) {
            std::cout << "Packet data error\n";
        }
    }

    void Server::OnReceive(std::function<void(Utils::Signal, std::shared_ptr<Utils::InputMemory>)> fn) {
        this->mReceiveCallBack = fn;
    }

    bool Server::Send(std::shared_ptr<Utils::Packet> packet) {
        for (int i = 0; i < MaxGuests; ++i) {
            if (mGuests[i] == 0) break;
            auto packetData = packet->Data();
            auto packetSize = packet->OutputDataSize();
            SDLNet_TCP_Send(mGuests[i], packetData, packetSize);
        }
        return true;
    }

    void Server::OnConnect(std::function<void(uint32_t)> fn) {
        mConnectCallBack = fn;
    }

    void Server::OnDisconnect(std::function<void(uint32_t)> fn) {
        mDisconnectCallBack = fn;
    }

    bool Server::SendTo(uint32_t index, std::shared_ptr<Utils::Packet> packet) {
        if (mGuests[index] == 0) return false;
        auto packetData = packet->Data();
        auto packetSize = packet->OutputDataSize();
        SDLNet_TCP_Send(mGuests[index], packetData, packetSize);
        return true;
    }

}