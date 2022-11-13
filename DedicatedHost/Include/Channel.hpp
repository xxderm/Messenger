#pragma once
#include "Packet.hpp"
#include "Message.hpp"

namespace DedicatedHost::Model {

    class Channel;
    using ChannelPtr = std::shared_ptr<Channel>;
    using Channels = std::vector<ChannelPtr>;

    class Channel final {
    public:
        Channel() = default;

        explicit Channel(Utils::NewChannelPacket packet);

        explicit Channel(std::string title, uint32_t places, bool access);

        void AddMessage(MessagePtr message);

        void UserConnect(uint32_t uid, UserPtr user);

        void UserDisconnect(uint32_t uid);

        std::string GetTitle() const { return mTitle; }

        uint32_t GetPlaces() const { return mPlaces; }

        bool GetAccess() const { return mAccess; }

        uint32_t GetAdminId() const { return mAdminId; }
    private:
        uint32_t mAdminId{};
        std::string mPass{};
        Messages mMessages;
        Users mUsers;
        std::string mTitle{};
        uint32_t mPlaces{};
        bool mAccess{};
    };

}