#pragma once
#include <string>
#include <memory>
#include "MemoryStream.hpp"

constexpr uint32_t DEFAULT_BUFFER_LEN = 250;

namespace Utils {

    enum Signal {
        CONNECT = 0,
        DISCONNECT,
        PING,
        CHANNELS,
        USERS,
        ADD_CHANNEL,
        DELETE_CHANNEL,
        UPDATE_CHANNEL,
        PERSONAL_MESSAGE,
        CHANNEL_MESSAGES
    };

    class IPacket {
    public:
        virtual void* Data() = 0;
        virtual ~IPacket() {}
    };

    class ConnectPacket : public IPacket {
    public:
        explicit ConnectPacket(const InputMemory& input) {

        }

        void* Data() override {

        }
    private:
        std::string mUserName;
    };

    class PacketManager final {
    public:
        std::shared_ptr<IPacket> GetPacket(const void* data) {
            InputMemory input((char*)&data);
            Signal signal;
            input.Read(&signal, sizeof(signal));
            switch (signal) {
                case Signal::CONNECT:
                    return std::make_shared<ConnectPacket>(input);
                case Signal::DISCONNECT:
                    break;
                case Signal::PING:
                    break;
                case Signal::CHANNELS:
                    break;
                case Signal::USERS:
                    break;
                case Signal::ADD_CHANNEL:
                    break;
                case Signal::DELETE_CHANNEL:
                    break;
                case Signal::UPDATE_CHANNEL:
                    break;
                case Signal::PERSONAL_MESSAGE:
                    break;
                case Signal::CHANNEL_MESSAGES:
                    break;
            }
        }
    };

}