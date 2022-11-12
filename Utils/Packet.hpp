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
        CHANNELS,
        NEW_CHANNEL,
        CLOSE_CHANNEL,
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

    struct ChannelPacketProperties final {
        std::string Name;
        bool Access = true;
        uint32_t Places;
    };

    class ChannelsPacket : public IPacket {
    public:
        explicit ChannelsPacket(std::vector<std::shared_ptr<ChannelPacketProperties>> channels) : mChannels(channels) {}

        explicit ChannelsPacket(std::shared_ptr<InputMemory> input) {
            input->Read(&mChannelsCount, sizeof(uint32_t));
            for (int i = 0; i < mChannelsCount; ++i) {
                std::string Name{};
                bool Access{};
                uint32_t Places{};
                uint32_t nameSize = 0;
                input->Read(&nameSize, sizeof(uint32_t));
                Name.resize(nameSize);
                input->Read(&Name[0], nameSize);
                input->Read(&Access, sizeof(bool));
                input->Read(&Places, sizeof(uint32_t));
                auto channel = std::make_shared<ChannelPacketProperties>();
                channel->Name = Name;
                channel->Access = Access;
                channel->Places = Places;
                mChannels.push_back(channel);
            }
        }

        char* Data() override {
            auto signal = Signal::CHANNELS;
            mChannelsCount = mChannels.size();
            uint32_t totalCharCount = 0;
            for(auto& channel : mChannels)
                totalCharCount += channel->Name.size();
            uint32_t packetSize = sizeof(Signal) +
                    sizeof(uint32_t) +
                    (mChannelsCount * sizeof(uint32_t)) +
                    (mChannelsCount * sizeof(bool)) +
                    (mChannelsCount * sizeof(uint32_t)) +
                    totalCharCount;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&mChannelsCount, sizeof(uint32_t));
            for(auto& channel : mChannels) {
                auto [name, access, places] = *channel.get();
                auto nameSize = name.size();
                mOutput->Write(&nameSize, sizeof(uint32_t));
                mOutput->Write(&name[0], nameSize);
                mOutput->Write(&access, sizeof(bool));
                mOutput->Write(&places, sizeof(uint32_t));
            }
            return mOutput->GetData();
        }

        std::vector<std::shared_ptr<ChannelPacketProperties>> GetChannels() { return mChannels; }
    private:
        uint32_t mChannelsCount{};
        std::vector<std::shared_ptr<ChannelPacketProperties>> mChannels;
        std::shared_ptr<OutputMemory> mOutput;
    };

    class NewChannelPacket : public IPacket {
    public:
        explicit NewChannelPacket(std::shared_ptr<ChannelPacketProperties> channel) : mChannel(channel) {}

        explicit NewChannelPacket(std::shared_ptr<InputMemory> input) {
            mChannel = std::make_shared<ChannelPacketProperties>();
            uint32_t nameSize = 0;
            input->Read(&nameSize, sizeof(uint32_t));
            mChannel->Name.resize(nameSize);
            input->Read(&mChannel->Name[0], nameSize);
            input->Read(&mChannel->Access, sizeof(bool));
            input->Read(&mChannel->Places, sizeof(uint32_t));
        }

        char* Data() override {
            auto signal = Signal::NEW_CHANNEL;
            uint32_t channelNameSize = mChannel->Name.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + channelNameSize + sizeof(bool) + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&channelNameSize, sizeof(uint32_t));
            mOutput->Write(&mChannel->Name[0], channelNameSize);
            mOutput->Write(&mChannel->Access, sizeof(bool));
            mOutput->Write(&mChannel->Places, sizeof(uint32_t));
            return mOutput->GetData();
        }

        std::shared_ptr<ChannelPacketProperties> GetChannel() { return mChannel; }
    private:
        std::shared_ptr<OutputMemory> mOutput;
        std::shared_ptr<ChannelPacketProperties> mChannel;
    };

    class CloseChannelPacket : public IPacket {
    public:
        explicit CloseChannelPacket(std::string channelName) : mChannelName(channelName) {}

        explicit CloseChannelPacket(std::shared_ptr<InputMemory> input) {
            uint32_t nameSize = 0;
            input->Read(&nameSize, sizeof(uint32_t));
            mChannelName.resize(nameSize);
            input->Read(&mChannelName[0], nameSize);
        }

        char* Data() override  {
            auto signal = Signal::CLOSE_CHANNEL;
            uint32_t nameSize = mChannelName.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + nameSize;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&nameSize, sizeof(uint32_t));
            mOutput->Write(&mChannelName[0], nameSize);
            return mOutput->GetData();
        }

        std::string GetChannelName() { return mChannelName; }
    private:
        std::shared_ptr<OutputMemory> mOutput;
        std::string mChannelName{};
    };

    class SignalManager final {
    public:
        static std::pair<Signal, std::shared_ptr<InputMemory>> GetSignal(char* data) {
            std::shared_ptr<InputMemory> input = std::make_shared<InputMemory>(data);
            Signal signal;
            input->Read(&signal, sizeof(Signal));
            return std::pair<Signal, std::shared_ptr<InputMemory>>(signal, input);
        }
    };

}