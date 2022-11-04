#include "../Include/Server.hpp"

namespace DedicatedHost {

    bool Server::Initialize(const char* port) {
        int iResult{};
        iResult = WSAStartup(MAKEWORD(2, 2), &mWSAData);

        if (iResult != 0) {
            std::cout << "WSAStartup failed: " << iResult << std::endl;
            return false;
        }

        struct addrinfo *result = nullptr;
        struct addrinfo *ptr = nullptr;
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET; // IpV4
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP; // TCP
        hints.ai_flags = AI_PASSIVE;

        iResult = getaddrinfo(nullptr, port, &hints, &result);
        if (iResult != 0) {
            std::cout << "Getaddrinfo failed: " << iResult << std::endl;
            WSACleanup();
            return false;
        }
        
        mServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (mServerSocket == INVALID_SOCKET) {
            std::cout << "Failed to create server socket: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        iResult = bind(mServerSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            std::cout << "Failed to create server socket: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            closesocket(mServerSocket);
            WSACleanup();
            return false;
        }

        if (listen(mServerSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
            closesocket(mServerSocket);
            WSACleanup();
            return false;
        }

        freeaddrinfo(result);
        return true;
    }

    void Server::Launch() noexcept {
        mRunning = true;
        fd_set active, read;
        FD_ZERO (&active);
        FD_SET (mServerSocket, &active);
        while (mRunning) {
            read = active;
            auto count = select(0, &read, 0, 0, 0);
            for (int i = 0; i < count; ++i) {
                auto socket = read.fd_array[i];
                if (FD_ISSET(socket, &read)) {
                    if (socket == mServerSocket) {
                        // New guest
                        SOCKET guest;
                        guest = accept(mServerSocket, 0, 0);
                        if (guest < 0) {
                            std::cout << "Failed to accept connection\n";
                        }
                        FD_SET(guest, &active);
                    }
                    else {
                        // New packet
                        char buffer[255];
                        auto n = recv(socket, buffer, 255, 0);

                        if (n > 0) {

                        }
                        else if (n == 0) {
                            std::cout << "Connection closed\n";
                            closesocket(socket);
                            FD_CLR(socket, &active);
                        }
                        else
                            std::cout << "Receive failed\n";

                    }
                }
            }
        }
        closesocket(mServerSocket);
    }

    Server::~Server() {
        WSACleanup();
    }

    void Server::Stop() noexcept {
        mRunning = false;
    }

}