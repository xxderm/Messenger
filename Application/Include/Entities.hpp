#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Application::Model {

    struct Guest final {
        uint32_t Id;
        std::string Name;
    };

    struct Message final {
        Guest User;
        std::string Content;
    };

    struct Channel final {
        std::vector<Message> Messages;
        std::vector<Guest> Guests;
        std::string Title{};
        uint32_t Places{};
        bool Access{};
    };

}