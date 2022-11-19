#pragma once
#include "User.hpp"
#include <vector>

namespace DedicatedHost::Model {

    class Message;
    using MessagePtr = std::shared_ptr<Message>;
    using Messages = std::vector<MessagePtr>;

    class Message final {
    public:
        explicit Message(UserPtr user, std::string msg);

        UserPtr GetUser() const { return mUser; }

        std::string GetContent() const { return mMessage; }
    private:
        UserPtr mUser;
        std::string mMessage{};
    };

}