#pragma once
#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include <algorithm>
#include "Packet.hpp"

namespace DedicatedHost::Controller {

    class ServerController final {
    public:
        ServerController();
    private:
        void ConnectUser(uint32_t index);
        void DisconnectUser(uint32_t index);
        void HandleReceive(Utils::Signal signal, std::shared_ptr<Utils::InputMemory> input);
        void AcceptNewUser(std::shared_ptr<Utils::InputMemory> input);
        void AcceptCloseUser(std::shared_ptr<Utils::InputMemory> input);
        void SendGuests();
        void SendChannels();
        void AcceptNewChannel(std::shared_ptr<Utils::InputMemory> input);
        void AcceptCloseChannel(std::shared_ptr<Utils::InputMemory> input);
        void GuestInfoUpdate(std::shared_ptr<Utils::InputMemory> input);
        void SendPM(std::shared_ptr<Utils::InputMemory> input);
        void ConnectToChannel(std::shared_ptr<Utils::InputMemory> input);
        void DisconnectFromChannel(std::shared_ptr<Utils::InputMemory> input);
        void ChannelMessage(std::shared_ptr<Utils::InputMemory> input);
        void SendChannelInfo();
    private:
        std::shared_ptr<Model::Server> mServer;
        Model::Users mUsers;
        Model::Channels mChannels;
    };

}