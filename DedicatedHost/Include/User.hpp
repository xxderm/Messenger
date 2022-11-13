#pragma once
#include <map>
#include <string>
#include <memory>
#include "Packet.hpp"

namespace DedicatedHost::Model {

    class User;
    using UserPtr = std::shared_ptr<User>;
    using Users = std::map<uint32_t, UserPtr>;

    class User final {
    public:
        User() = default;

        explicit User(uint32_t id, std::string name);

        explicit User(Utils::ConnectPacket connectedUser);

        void SetId(uint32_t id);

        void SetName(std::string name);

        uint32_t GetId() const { return mId; }

        std::string GetName() const { return mName; }
    private:
        uint32_t mId{};
        std::string mName{};
    };

}