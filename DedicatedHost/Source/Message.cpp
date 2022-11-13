#include "Message.hpp"

namespace DedicatedHost::Model {

    Message::Message(UserPtr user, std::string msg) : mUser(user), mMessage(msg) {}

}