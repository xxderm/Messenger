#include "User.hpp"

namespace DedicatedHost::Model {

    User::User(uint32_t id, std::string name) : mId(id), mName(name) {

    }

    void User::SetId(uint32_t id) {
        this->mId = id;
    }

    void User::SetName(std::string name) {
        this->mName = name;
    }

    User::User(Utils::ConnectPacket connectedUser) {
        this->mId = connectedUser.GetId();
        this->mName = connectedUser.GetName();
    }

}