#include <gtest/gtest.h>
#include "../Utils/Packet.hpp"

TEST(PacketTest, ConnectPacket) {
    Utils::ConnectPacket connectPacket("UserNameTest");
    auto sig =  Utils::SignalManager::GetSignal(connectPacket.Data());
    Utils::ConnectPacket pack(sig.second);
    ASSERT_EQ("UserNameTest",pack.GetName());
    ASSERT_EQ(connectPacket.GetName(), pack.GetName());
}

TEST(PacketTest, GuestsPacket) {
    std::vector<std::string> guests = {
            "Guest1",
            "Guest2",
            "Guest3",
            "GuestTest",
            "Guest4"
    };
    Utils::GuestsPacket guestsPacket(guests);
    auto sig =  Utils::SignalManager::GetSignal(guestsPacket.Data());
    Utils::GuestsPacket pack(sig.second);
    ASSERT_EQ(pack.GetGuests(), guests);
}

TEST(PacketTest, ChannelsPacket) {
    srand(time(0));
    std::vector<std::shared_ptr<Utils::ChannelPacketProperties>> channels;
    for (int i = 0; i < 50; ++i) {
        std::string name = "Channel#" + std::to_string(rand() % 500);
        bool Access = static_cast<bool>(rand() % 2);
        uint32_t Places = rand() % 64;
        auto channel = std::make_shared<Utils::ChannelPacketProperties>();
        channel->Name = name;
        channel->Access = Access;
        channel->Places = Places;
        channels.push_back(channel);
    }
    Utils::ChannelsPacket channelsPacket(channels);
    auto sig = Utils::SignalManager::GetSignal(channelsPacket.Data());
    Utils::ChannelsPacket pack(sig.second);
    auto channelsFromPacket = pack.GetChannels();
    ASSERT_EQ(channels.size(), channelsFromPacket.size());
    for (int i = 0; i < channelsFromPacket.size(); ++i) {
        ASSERT_EQ(channels[i]->Name, channelsFromPacket[i]->Name);
        ASSERT_EQ(channels[i]->Access, channelsFromPacket[i]->Access);
        ASSERT_EQ(channels[i]->Places, channelsFromPacket[i]->Places);
    }
}

TEST(PacketTest, NewChannelPacket) {
    srand(time(0));
    std::shared_ptr<Utils::ChannelPacketProperties> channel;
    channel = std::make_shared<Utils::ChannelPacketProperties>();
    channel->Name = "Channel#" + std::to_string(rand() % 500);
    channel->Access = static_cast<bool>(rand() % 2);
    channel->Places = rand() % 64;

    Utils::NewChannelPacket newChannelPacket(channel);
    auto sig = Utils::SignalManager::GetSignal(newChannelPacket.Data());
    Utils::NewChannelPacket pack(sig.second);
    auto channelFromPacket = pack.GetChannel();
    ASSERT_EQ(channel->Name, channelFromPacket->Name);
    ASSERT_EQ(channel->Access, channelFromPacket->Access);
    ASSERT_EQ(channel->Places, channelFromPacket->Places);
}

TEST(PacketTest, CloseChannelPacket) {
    auto closeChannelName = "Channel#24";
    Utils::CloseChannelPacket closeChannelPacket(closeChannelName);
    auto sig = Utils::SignalManager::GetSignal(closeChannelPacket.Data());

    Utils::CloseChannelPacket pack(sig.second);
    auto closeName = pack.GetChannelName();
    ASSERT_EQ(closeChannelName, closeName);
}

TEST(PacketTest, GuestUpdatePack) {
    auto guestName = "Guest1";
    uint32_t guestId = 1;
    Utils::GuestUpdatePacket guestUpdatePacket(guestName, guestId);
    auto sig = Utils::SignalManager::GetSignal(guestUpdatePacket.Data());

    Utils::GuestUpdatePacket pack(sig.second);
    auto packGuestName = pack.GetName();
    auto packGuestId = pack.GetId();
    ASSERT_EQ(guestName, packGuestName);
    ASSERT_EQ(guestId, packGuestId);
}

TEST(PacketTest, PMPacket) {
    auto message = "Hello";
    uint32_t fromId = 1;
    uint32_t toId = 2;
    Utils::PersonalMessagePacket personalMessagePacket(fromId, toId, message);
    auto sig = Utils::SignalManager::GetSignal(personalMessagePacket.Data());

    Utils::PersonalMessagePacket pack(sig.second);
    auto packMsg = pack.GetMessage();
    auto packFromId = pack.GetFrom();
    auto packToId = pack.GetTo();
    ASSERT_EQ(message, packMsg);
    ASSERT_EQ(fromId, packFromId);
    ASSERT_EQ(toId, packToId);
}