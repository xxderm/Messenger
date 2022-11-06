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
        virtual char* Data() = 0;
        virtual ~IPacket() {}
    };

    class ConnectPacket : public IPacket {
    public:
        explicit ConnectPacket(const std::string& userName) {
            mUserName = userName;
            mNameLen = userName.size();
        }

        explicit ConnectPacket(InputMemory& input) {
            input.Read(&mNameLen, sizeof(mNameLen));
            mUserName.resize(mNameLen);
            input.Read(&mUserName[0], mNameLen);
        }

        char* Data() override {
            uint32_t packetSize = sizeof(mNameLen) + mNameLen;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&mNameLen, sizeof(mNameLen));
            mOutput->Write(&mUserName[0], mNameLen);
            return mOutput->GetData();
        }
    private:
        std::string mUserName;
        uint32_t mNameLen{};
        std::shared_ptr<OutputMemory> mOutput;
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