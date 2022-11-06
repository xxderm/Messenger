#pragma once
#include <string>
#include <memory>
#include <utility>
#include "MemoryStream.hpp"

constexpr uint32_t DEFAULT_BUFFER_LEN = 250;

namespace Utils {

    enum Signal {
        CONNECT = 0,
        GUESTS,
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

        explicit ConnectPacket(std::shared_ptr<InputMemory> input) {
            input->Read(&mNameLen, sizeof(mNameLen));
            mUserName.resize(mNameLen);
            input->Read(&mUserName[0], mNameLen);
        }

        char* Data() override {
            auto signal = Signal::CONNECT;
            uint32_t packetSize = sizeof(Signal) + sizeof(mNameLen) + mNameLen;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&mNameLen, sizeof(mNameLen));
            mOutput->Write(&mUserName[0], mNameLen);
            return mOutput->GetData();
        }

        std::string GetName() const { return mUserName; }
    private:
        std::string mUserName;
        uint32_t mNameLen{};
        std::shared_ptr<OutputMemory> mOutput;
    };

    class GuestsPacket : public IPacket {
    public:
        explicit GuestsPacket(std::vector<std::string> guests) : mGuests(std::move(guests)) {}

        explicit GuestsPacket(std::shared_ptr<InputMemory> input) {
            uint32_t guestCount = 0;
            uint32_t nameSize = 0;
            input->Read(&guestCount, sizeof(uint32_t));
            for (int i = 0; i < guestCount; ++i) {
                std::string name{};
                input->Read(&nameSize, sizeof(uint32_t));
                name.resize(nameSize);
                input->Read(&name[0], nameSize);
                mGuests.push_back(name);
            }
        }

        char* Data() override {
            auto signal = Signal::GUESTS;
            auto guestCount = mGuests.size();
            uint32_t totalCharCount = 0;
            for(auto& name : mGuests)
                totalCharCount += name.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + (sizeof(uint32_t) * guestCount) + totalCharCount;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&guestCount, sizeof(uint32_t));
            for(auto& name : mGuests) {
                auto nameSize = name.size();
                mOutput->Write(&nameSize, sizeof(uint32_t));
                mOutput->Write(&name[0], nameSize);
            }
            return mOutput->GetData();
        }

        std::vector<std::string> GetGuests() const { return mGuests; }
    private:
        std::vector<std::string> mGuests;
        std::shared_ptr<OutputMemory> mOutput;
    };

    class SignalManager final {
    public:
        static std::pair<Signal, std::shared_ptr<InputMemory>> GetPacket(char* data) {
            std::shared_ptr<InputMemory> input = std::make_shared<InputMemory>(data);
            Signal signal;
            input->Read(&signal, sizeof(Signal));
            return std::pair<Signal, std::shared_ptr<InputMemory>>(signal, input);
        }
    };

}