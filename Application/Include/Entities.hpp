#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "Packet.hpp"

namespace Application::Model {

    struct Guest final {
        Guest(std::shared_ptr<Utils::GuestProperties> guest) {
            this->Name = guest->Name;
            this->Id = guest->Id;
        }
        Guest() = default;
        uint32_t Id;
        std::string Name;
    };

    struct Message final {
        Message() = default;
        Guest User;
        std::string Content{};
    };

    using Guests = std::vector<Guest>;
    using Messages = std::vector<Message>;

    struct Channel final {
        Channel(std::shared_ptr<Utils::ChannelPacketProperties> channel) {
            this->Title = channel->Name;
            this->Places = channel->Places;
            this->Access = channel->Access;
        }

        Messages UsrMessages;
        Guests Users;
        std::string Title{};
        uint32_t Places{};
        bool Access{};
    };

    using Channels = std::vector<Channel>;

}