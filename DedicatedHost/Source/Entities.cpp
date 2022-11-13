#include "Channel.hpp"

namespace DedicatedHost::Model {

    Channel::Channel(std::string title, uint32_t places, bool access) : mTitle(title), mPlaces(places), mAccess(access) {

    }

    void Channel::UserConnect(uint32_t uid, UserPtr user) {
        if (mUsers.count(uid) == 0)
            mUsers.insert(std::pair<uint32_t, UserPtr>(uid, user));
    }

    void Channel::UserDisconnect(uint32_t uid) {
        mUsers.erase(uid);
    }

    void Channel::AddMessage(MessagePtr message) {
        mMessages.push_back(message);
    }

    Channel::Channel(Utils::NewChannelPacket packet) {
        this->mTitle = packet.GetChannel()->Name;
        this->mPlaces = packet.GetChannel()->Places;
        this->mAccess = packet.GetChannel()->Access;
        this->mAdminId = packet.GetAdminId();
    }

}