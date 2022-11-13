#include <gtest/gtest.h>
#include "../Utils/Packet.hpp"

TEST(PacketTest, ConnectPacket) {
    Utils::ConnectPacket connectPacket("UserNameTest");
    auto sig =  Utils::SignalManager::GetSignal(connectPacket.Data());
    Utils::ConnectPacket pack(sig.second);
    ASSERT_EQ("UserNameTest",pack.GetName());
    ASSERT_EQ(connectPacket.GetName(), pack.GetName());
    ASSERT_EQ(pack.GetId(), connectPacket.GetId());
}

TEST(PacketTest, GuestsPacket) {
    std::vector<std::shared_ptr<Utils::GuestProperties>> guests;
    guests.push_back(std::make_shared<Utils::GuestProperties>("Guest", 0));
    guests.push_back(std::make_shared<Utils::GuestProperties>("Guest1", 1));
    guests.push_back(std::make_shared<Utils::GuestProperties>("Guest2", 2));
    Utils::GuestsPacket guestsPacket(guests);
    auto sig =  Utils::SignalManager::GetSignal(guestsPacket.Data());
    Utils::GuestsPacket pack(sig.second);
    ASSERT_EQ(pack.GetGuests().size(), guests.size());
    for(int i = 0; i < guests.size(); ++i) {
        ASSERT_EQ(guests[i]->Id, pack.GetGuests()[i]->Id);
        ASSERT_EQ(guests[i]->Name, pack.GetGuests()[i]->Name);
    }
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
    uint32_t adminId = 0;

    Utils::NewChannelPacket newChannelPacket(channel, adminId);
    auto sig = Utils::SignalManager::GetSignal(newChannelPacket.Data());
    Utils::NewChannelPacket pack(sig.second);
    auto channelFromPacket = pack.GetChannel();
    ASSERT_EQ(channel->Name, channelFromPacket->Name);
    ASSERT_EQ(channel->Access, channelFromPacket->Access);
    ASSERT_EQ(channel->Places, channelFromPacket->Places);
    ASSERT_EQ(adminId, pack.GetAdminId());
}

TEST(PacketTest, CloseChannelPacket) {
    auto closeChannelName = "Channel#24";
    uint32_t uid = 0;
    Utils::CloseChannelPacket closeChannelPacket(closeChannelName, uid);
    auto sig = Utils::SignalManager::GetSignal(closeChannelPacket.Data());

    Utils::CloseChannelPacket pack(sig.second);
    auto closeName = pack.GetChannelName();
    ASSERT_EQ(closeChannelName, closeName);
    ASSERT_EQ(uid, pack.GetUserId());
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

TEST(PacketTest, ChnlConnectPack) {
    auto channelName = "Channel#24";
    uint32_t userId = 0;
    Utils::ChannelConnectPacket channelConnectPacket(userId, channelName);
    auto sig = Utils::SignalManager::GetSignal(channelConnectPacket.Data());

    Utils::ChannelConnectPacket pack(sig.second);
    auto packChannelName = pack.GetChannel();
    auto packUsrId = pack.GetUserId();
    ASSERT_EQ(channelName, packChannelName);
    ASSERT_EQ(userId, packUsrId);
}

TEST(PacketTest, DiscPack) {
    uint32_t userId = 0;
    Utils::DisconnectPacket disconnectPacket(userId);
    auto sig = Utils::SignalManager::GetSignal(disconnectPacket.Data());

    Utils::DisconnectPacket pack(sig.second);
    auto packUsrId = pack.GetUserId();
    ASSERT_EQ(userId, packUsrId);
}

TEST(PacketTest, ChnlLeavePack) {
    uint32_t userId = 0;
    auto channelName = "Channel#1";
    Utils::ChannelLeavePacket channelLeavePacket(userId, channelName);
    auto sig = Utils::SignalManager::GetSignal(channelLeavePacket.Data());

    Utils::ChannelLeavePacket pack(sig.second);
    auto packUserId = pack.GetUserId();
    auto packChannel = pack.GetChannelName();
    ASSERT_EQ(userId, packUserId);
    ASSERT_EQ(channelName, packChannel);
}

TEST(PacketTest, ChnlMessagePack) {
    uint32_t userId = 0;
    auto channelName = "Channel#1";
    auto message = "Hello";
    Utils::ChannelMessagePacket channelMessagePacket(userId, channelName, message);
    auto sig = Utils::SignalManager::GetSignal(channelMessagePacket.Data());

    Utils::ChannelMessagePacket pack(sig.second);
    auto packUserId = pack.GetUserId();
    auto packChannel = pack.GetChannelName();
    auto packMessage = pack.GetMessage();
    ASSERT_EQ(userId, packUserId);
    ASSERT_EQ(channelName, packChannel);
    ASSERT_EQ(message, packMessage);
}