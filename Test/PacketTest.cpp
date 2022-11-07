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
    std::vector<std::shared_ptr<Utils::ChannelsPacketProperties>> channels;
    for (int i = 0; i < 50; ++i) {
        std::string name = "Channel#" + std::to_string(rand() % 500);
        bool Access = static_cast<bool>(rand() % 2);
        uint32_t Places = rand() % 64;
        auto channel = std::make_shared<Utils::ChannelsPacketProperties>();
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