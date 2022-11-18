#pragma once
#include <string>
#include <memory>
#include <utility>
#include "MemoryStream.hpp"
#include <vector>

constexpr uint32_t DEFAULT_BUFFER_LEN = 1024;

namespace Utils {

    enum Signal {
        CONNECT = 0,
        GUESTS,
        CHANNELS,
        NEW_CHANNEL,
        CLOSE_CHANNEL,
        GUEST_INFO_UPDATE,
        PERSONAL_MESSAGE,
        CONNECT_CHANNEL,
        DISCONNECT,
        CHANNEL_LEAVE,
        CHANNEL_MESSAGE,
        SERVER_STOP
    };

    class Packet {
    public:
        virtual char* Data() = 0;
        virtual uint32_t OutputDataSize() = 0;
        virtual ~Packet() {}
    protected:
        std::shared_ptr<OutputMemory> mOutput;
    };

    class PacketRequest : public Packet  {
    public:
        explicit PacketRequest(Signal signal) : mSignal(signal) {}

        char* Data() override {
            uint32_t packetSize = sizeof(Signal);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&mSignal, sizeof(Signal));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        Signal GetSignal() const { return mSignal; }
    private:
        Signal mSignal;
    };

    class ConnectPacket : public Packet {
    public:
        explicit ConnectPacket(const std::string& userName, uint32_t id = 0) {
            mUserName = userName;
            mNameLen = userName.size();
            mId = id;
        }

        explicit ConnectPacket(std::shared_ptr<InputMemory> input) {
            input->Read(&mNameLen, sizeof(mNameLen));
            mUserName.resize(mNameLen);
            input->Read(&mUserName[0], mNameLen);
            input->Read(&mId, sizeof(uint32_t));
        }

        char* Data() override {
            auto signal = Signal::CONNECT;
            uint32_t packetSize = sizeof(Signal) + sizeof(mNameLen) + mNameLen + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&mNameLen, sizeof(mNameLen));
            mOutput->Write(&mUserName[0], mNameLen);
            mOutput->Write(&mId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::string GetName() const { return mUserName; }

        uint32_t GetId() const { return mId; }
    private:
        std::string mUserName;
        uint32_t mNameLen{};
        uint32_t mId{};
    };

    struct GuestProperties final {
        GuestProperties() = default;

        GuestProperties(std::string name, uint32_t id) : Name(name), Id(id) {}

        std::string Name{};
        uint32_t Id{};
    };

    class GuestsPacket : public Packet {
    public:
        explicit GuestsPacket(std::vector<std::shared_ptr<GuestProperties>> guests) : mGuests(std::move(guests)) {}

        explicit GuestsPacket(std::shared_ptr<InputMemory> input) {
            uint32_t guestCount = 0;
            uint32_t nameSize = 0;
            input->Read(&guestCount, sizeof(uint32_t));
            for (int i = 0; i < guestCount; ++i) {
                std::shared_ptr<GuestProperties> guest = std::make_shared<GuestProperties>();
                std::string name{};
                uint32_t id{};
                input->Read(&nameSize, sizeof(uint32_t));
                name.resize(nameSize);
                input->Read(&name[0], nameSize);
                input->Read(&id, sizeof(uint32_t));
                guest->Name = name;
                guest->Id = id;
                mGuests.push_back(guest);
            }
        }

        char* Data() override {
            auto signal = Signal::GUESTS;
            auto guestCount = mGuests.size();
            uint32_t totalCharCount = 0;
            for(auto& guest : mGuests)
                totalCharCount += guest->Name.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) +
                    (sizeof(uint32_t) * guestCount) +
                    (sizeof(uint32_t) * guestCount) + totalCharCount;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&guestCount, sizeof(uint32_t));
            for(auto& guest : mGuests) {
                auto nameSize = guest->Name.size();
                mOutput->Write(&nameSize, sizeof(uint32_t));
                mOutput->Write(&guest->Name[0], nameSize);
                mOutput->Write(&guest->Id, sizeof(uint32_t));
            }
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::vector<std::shared_ptr<GuestProperties>> GetGuests() const { return mGuests; }
    private:
        std::vector<std::shared_ptr<GuestProperties>> mGuests;
    };

    struct ChannelPacketProperties final {
        std::string Name;
        bool Access = true;
        uint32_t Places;
    };

    class ChannelsPacket : public Packet {
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

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::vector<std::shared_ptr<ChannelPacketProperties>> GetChannels() { return mChannels; }
    private:
        uint32_t mChannelsCount{};
        std::vector<std::shared_ptr<ChannelPacketProperties>> mChannels;
    };

    class NewChannelPacket : public Packet {
    public:
        explicit NewChannelPacket(std::shared_ptr<ChannelPacketProperties> channel, uint32_t adminId)
            : mChannel(channel),  mAdminId(adminId) {}

        explicit NewChannelPacket(std::shared_ptr<InputMemory> input) {
            mChannel = std::make_shared<ChannelPacketProperties>();
            uint32_t nameSize = 0;
            input->Read(&nameSize, sizeof(uint32_t));
            mChannel->Name.resize(nameSize);
            input->Read(&mChannel->Name[0], nameSize);
            input->Read(&mChannel->Access, sizeof(bool));
            input->Read(&mChannel->Places, sizeof(uint32_t));
            input->Read(&mAdminId, sizeof(uint32_t));
        }

        char* Data() override {
            auto signal = Signal::NEW_CHANNEL;
            uint32_t channelNameSize = mChannel->Name.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) +
                    channelNameSize + sizeof(bool) + sizeof(uint32_t) + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&channelNameSize, sizeof(uint32_t));
            mOutput->Write(&mChannel->Name[0], channelNameSize);
            mOutput->Write(&mChannel->Access, sizeof(bool));
            mOutput->Write(&mChannel->Places, sizeof(uint32_t));
            mOutput->Write(&mAdminId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::shared_ptr<ChannelPacketProperties> GetChannel() { return mChannel; }

        uint32_t GetAdminId() const { return mAdminId; }
    private:
        std::shared_ptr<ChannelPacketProperties> mChannel;
        uint32_t mAdminId{};
    };

    class CloseChannelPacket : public Packet {
    public:
        explicit CloseChannelPacket(std::string channelName, uint32_t uid) : mChannelName(channelName), mUserId(uid) {}

        explicit CloseChannelPacket(std::shared_ptr<InputMemory> input) {
            uint32_t nameSize = 0;
            input->Read(&nameSize, sizeof(uint32_t));
            mChannelName.resize(nameSize);
            input->Read(&mChannelName[0], nameSize);
            input->Read(&mUserId, sizeof(uint32_t));
        }

        char* Data() override  {
            auto signal = Signal::CLOSE_CHANNEL;
            uint32_t nameSize = mChannelName.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + nameSize + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&signal, sizeof(Signal));
            mOutput->Write(&nameSize, sizeof(uint32_t));
            mOutput->Write(&mChannelName[0], nameSize);
            mOutput->Write(&mUserId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::string GetChannelName() { return mChannelName; }

        uint32_t GetUserId() const { return mUserId; }
    private:
        std::string mChannelName{};
        uint32_t mUserId{};
    };

    class GuestUpdatePacket : public Packet {
    public:
        explicit GuestUpdatePacket(std::string guestName, uint32_t guestId) : mGuestName(guestName), mGuestId(guestId) {}

        explicit GuestUpdatePacket(std::shared_ptr<InputMemory> input) {
            uint32_t nameSize = 0;
            input->Read(&nameSize, sizeof(uint32_t));
            mGuestName.resize(nameSize);
            input->Read(&mGuestName[0], nameSize);
            input->Read(&mGuestId, sizeof(uint32_t));
        }

        char* Data() override {
            auto sig = Signal::GUEST_INFO_UPDATE;
            uint32_t nameSize = mGuestName.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + nameSize + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&nameSize, sizeof(uint32_t));
            mOutput->Write(&mGuestName[0], nameSize);
            mOutput->Write(&mGuestId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::string GetName() { return mGuestName; }

        uint32_t GetId() { return mGuestId; }
    private:
        std::string mGuestName{};
        uint32_t mGuestId{};
    };

    class PersonalMessagePacket : public Packet {
    public:
        explicit PersonalMessagePacket(uint32_t from, uint32_t to, std::string msg) : mFrom(from), mTo(to), mMessage(msg) {}

        explicit PersonalMessagePacket(std::shared_ptr<InputMemory> input) {
            uint32_t messageSize = 0;
            input->Read(&messageSize, sizeof(uint32_t));
            mMessage.resize(messageSize);
            input->Read(&mMessage[0], messageSize);
            input->Read(&mFrom, sizeof(uint32_t));
            input->Read(&mTo, sizeof(uint32_t));
        }

        char* Data() override {
            auto sig = Signal::PERSONAL_MESSAGE;
            uint32_t messageSize = mMessage.size();
            uint32_t packSize = sizeof(Signal) + sizeof(uint32_t) + messageSize + (sizeof(uint32_t) * 2);
            mOutput = std::make_shared<OutputMemory>(packSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&messageSize, sizeof(uint32_t));
            mOutput->Write(&mMessage[0], messageSize);
            mOutput->Write(&mFrom, sizeof(uint32_t));
            mOutput->Write(&mTo, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::string GetMessage() { return mMessage; }

        uint32_t GetFrom() { return mFrom; }

        uint32_t GetTo() { return mTo; }
    private:
        uint32_t mFrom{};
        uint32_t mTo{};
        std::string mMessage{};
    };

    class ChannelConnectPacket : public Packet {
    public:
        explicit ChannelConnectPacket(uint32_t userId, std::string channelName, std::string pass = "")
            : mUserId(userId), mChannelName(channelName), mPass(pass) {}

        explicit ChannelConnectPacket(std::shared_ptr<InputMemory> input) {
            uint32_t channelNameSize = 0;
            input->Read(&channelNameSize, sizeof(uint32_t));
            mChannelName.resize(channelNameSize);
            input->Read(&mChannelName[0], channelNameSize);
            input->Read(&mUserId, sizeof(uint32_t));
        }

        char* Data() override {
            uint32_t channelNameSize = mChannelName.size();
            auto sig = Signal::CONNECT_CHANNEL;
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + channelNameSize + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&channelNameSize, sizeof(uint32_t));
            mOutput->Write(&mChannelName[0], channelNameSize);
            mOutput->Write(&mUserId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        std::string GetChannel() const { return mChannelName; }

        uint32_t GetUserId() const { return mUserId; }
    private:
        uint32_t mUserId{};
        std::string mChannelName{};
        std::string mPass{};
    };

    class DisconnectPacket : public Packet {
    public:
        explicit DisconnectPacket(uint32_t userId) : mUserId(userId) {}

        explicit DisconnectPacket(std::shared_ptr<InputMemory> input) {
            input->Read(&mUserId, sizeof(uint32_t));
        }

        char* Data() override {
            auto sig = Signal::DISCONNECT;
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t);
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&mUserId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        uint32_t GetUserId() const { return mUserId; }
    private:
        uint32_t mUserId{};
    };

    class ChannelLeavePacket : public Packet {
    public:
        explicit ChannelLeavePacket(uint32_t userId, std::string channel) : mUserId(userId), mChannelName(channel) {}

        explicit ChannelLeavePacket(std::shared_ptr<InputMemory> input) {
            uint32_t channelNameSize = 0;
            input->Read(&channelNameSize, sizeof(uint32_t));
            mChannelName.resize(channelNameSize);
            input->Read(&mChannelName[0], channelNameSize);
            input->Read(&mUserId, sizeof(uint32_t));
        }

        char* Data() override {
            auto sig = Signal::CHANNEL_LEAVE;
            uint32_t channelNameSize = mChannelName.size();
            uint32_t packetSize = sizeof(Signal) + sizeof(uint32_t) + sizeof(uint32_t) + channelNameSize;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&channelNameSize, sizeof(uint32_t));
            mOutput->Write(&mChannelName[0], channelNameSize);
            mOutput->Write(&mUserId, sizeof(uint32_t));
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        uint32_t GetUserId() const { return mUserId; }

        std::string GetChannelName() const { return mChannelName; }
    private:
        uint32_t mUserId{};
        std::string mChannelName{};
    };

    class ChannelMessagePacket : public Packet {
    public:
        explicit ChannelMessagePacket(uint32_t userId, std::string channelName, std::string msg)
            : mUserId(userId), mChannelName(channelName), mMessage(msg) {}

        explicit ChannelMessagePacket(std::shared_ptr<InputMemory> input) {
            uint32_t channelNameSize = 0;
            uint32_t messageSize = 0;
            input->Read(&channelNameSize, sizeof(uint32_t));
            input->Read(&messageSize, sizeof(uint32_t));
            mChannelName.resize(channelNameSize);
            mMessage.resize(messageSize);
            input->Read(&mUserId, sizeof(uint32_t));
            input->Read(&mChannelName[0], channelNameSize);
            input->Read(&mMessage[0], messageSize);
        }

        char* Data() override {
            uint32_t channelNameSize = mChannelName.size();
            uint32_t messageSize = mMessage.size();
            auto sig = Signal::CHANNEL_MESSAGE;
            uint32_t packetSize = sizeof(Signal) + (sizeof(uint32_t) * 3) + channelNameSize + messageSize;
            mOutput = std::make_shared<OutputMemory>(packetSize);
            mOutput->Write(&sig, sizeof(Signal));
            mOutput->Write(&channelNameSize, sizeof(uint32_t));
            mOutput->Write(&messageSize, sizeof(uint32_t));
            mOutput->Write(&mUserId, sizeof(uint32_t));
            mOutput->Write(&mChannelName[0], channelNameSize);
            mOutput->Write(&mMessage[0], messageSize);
            return mOutput->GetData();
        }

        uint32_t OutputDataSize() override { return mOutput->GetBufferSize(); }

        uint32_t GetUserId() const { return mUserId; }

        std::string GetChannelName() const { return mChannelName; }

        std::string GetMessage() const { return mMessage; }
    private:
        uint32_t mUserId{};
        std::string mChannelName{};
        std::string mMessage{};
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