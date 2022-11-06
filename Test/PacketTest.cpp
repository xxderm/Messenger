#include <gtest/gtest.h>
#include "../Utils/Packet.hpp"

TEST(ConnectPacketTest, BasicAssertions) {
    Utils::ConnectPacket connectPacket("UserNameTest");
    auto sig =  Utils::SignalManager::GetPacket(connectPacket.Data());
    Utils::ConnectPacket pack(sig.second);
    ASSERT_EQ("UserNameTest",pack.GetName());
    ASSERT_EQ(connectPacket.GetName(), pack.GetName());
}

TEST(GuestsPacketTest, BasicAssertions) {
    std::vector<std::string> guests = {
            "Guest1",
            "Guest2",
            "Guest3",
            "GuestTest",
            "Guest4"
    };
    Utils::GuestsPacket guestsPacket(guests);
    auto sig =  Utils::SignalManager::GetPacket(guestsPacket.Data());
    Utils::GuestsPacket pack(sig.second);
    ASSERT_EQ(pack.GetGuests(), guests);
}