#include "ServerController.hpp"

namespace DedicatedHost::Controller {

    ServerController::ServerController() {
        mServer = std::make_shared<Model::Server>();
        if (!mServer->Initialize(5000))
            std::cout << "Failed to start server\n";
        mServer->OnReceive([this](Utils::Signal signal, std::shared_ptr<Utils::InputMemory> input) {
            this->HandleReceive(signal, input);
        });
        mServer->OnConnect([this](uint32_t index){
           this->ConnectUser(index);
        });
        mServer->OnDisconnect([this](uint32_t index){
            this->DisconnectUser(index);
        });
        mServer->Launch();
    }

    void ServerController::HandleReceive(Utils::Signal signal, std::shared_ptr<Utils::InputMemory> input) {
        switch (signal) {
            case Utils::CONNECT:
                this->AcceptNewUser(input);
                break;
            case Utils::GUESTS:
                this->SendGuests();
                break;
            case Utils::CHANNELS:
                this->SendChannels();
                break;
            case Utils::NEW_CHANNEL:
                this->AcceptNewChannel(input);
                break;
            case Utils::CLOSE_CHANNEL:
                this->AcceptCloseChannel(input);
                break;
            case Utils::GUEST_INFO_UPDATE:
                this->GuestInfoUpdate(input);
                break;
            case Utils::PERSONAL_MESSAGE:
                this->SendPM(input);
                break;
            case Utils::CONNECT_CHANNEL:
                this->ConnectToChannel(input);
                break;
            case Utils::DISCONNECT:
                this->AcceptCloseUser(input);
                break;
            case Utils::CHANNEL_LEAVE:
                this->DisconnectFromChannel(input);
                break;
            case Utils::CHANNEL_MESSAGE:
                this->ChannelMessage(input);
                break;
        }
    }

    void ServerController::SendGuests() {
        std::vector<std::shared_ptr<Utils::GuestProperties>> guests;
        for (auto& user : mUsers) {
            Utils::GuestProperties guest;
            guest.Id = user.second->GetId();
            guest.Name = user.second->GetName();
            guests.push_back(std::make_shared<Utils::GuestProperties>(guest));
        }
        std::shared_ptr<Utils::Packet> packet = std::make_shared<Utils::GuestsPacket>(guests);
        mServer->Send(packet);
    }

    void ServerController::SendChannels() {
        std::vector<std::shared_ptr<Utils::ChannelPacketProperties>> channels;
        for (auto& channel : mChannels) {
            Utils::ChannelPacketProperties ch;
            ch.Name = channel->GetTitle();
            ch.Places = channel->GetPlaces();
            ch.Access = channel->GetAccess();
            channels.push_back(std::make_shared<Utils::ChannelPacketProperties>(ch));
        }
        std::shared_ptr<Utils::Packet> packet = std::make_shared<Utils::ChannelsPacket>(channels);
        mServer->Send(packet);
    }

    void ServerController::AcceptNewChannel(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::NewChannelPacket(input);
        for (auto& channel : mChannels)
            if (channel->GetTitle() == packet.GetChannel()->Name)
                return;
        mChannels.push_back(std::make_shared<Model::Channel>(packet));
    }

    void ServerController::AcceptCloseChannel(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::CloseChannelPacket(input);
        if (mChannels.empty())
            return;
        for (auto it = mChannels.begin(); it != mChannels.end(); ++it) {
            auto& ch = *it;
            if (ch->GetTitle() == packet.GetChannelName() && ch->GetAdminId() == packet.GetUserId()) {
                mChannels.erase(it);
                break;
            }
        }
    }

    void ServerController::GuestInfoUpdate(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::GuestUpdatePacket(input);
        if (mUsers.empty())
            return;
        if (mUsers.count(packet.GetId()) == 0)
            return;
        mUsers[packet.GetId()]->SetName(packet.GetName());
        this->SendGuests();
    }

    void ServerController::ConnectUser(uint32_t index) {

    }

    void ServerController::DisconnectUser(uint32_t index) {

    }

    void ServerController::SendPM(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::PersonalMessagePacket(input);
        mServer->SendTo(packet.GetFrom(), std::make_shared<Utils::PersonalMessagePacket>(input));
    }

    void ServerController::AcceptNewUser(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ConnectPacket(input);
        auto index = mUsers.size();
        mUsers.insert(std::pair<uint32_t, Model::UserPtr>(
                index,
                std::make_shared<Model::User>( packet )
        ));
        mServer->SendTo(index, std::make_shared<Utils::ConnectPacket>(packet));
    }

    void ServerController::ConnectToChannel(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ChannelConnectPacket(input);
        for (auto& channel : mChannels) {
            if (channel->GetTitle() == packet.GetChannel()) {
                channel->UserConnect(packet.GetUserId(), mUsers[packet.GetUserId()]);
                break;;
            }
        }
        this->SendChannels();
    }

    void ServerController::AcceptCloseUser(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::DisconnectPacket(input);
        mUsers.erase(packet.GetUserId());
        this->SendGuests();
    }

    void ServerController::DisconnectFromChannel(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ChannelLeavePacket(input);
        for (auto& channel : mChannels) {
            if (channel->GetTitle() == packet.GetChannelName()) {
                channel->UserDisconnect(packet.GetUserId());
            }
        }
        this->SendChannels();
    }

    void ServerController::ChannelMessage(std::shared_ptr<Utils::InputMemory> input) {
        auto packet = Utils::ChannelMessagePacket(input);
        auto uid = packet.GetUserId();
        auto channelTitle = packet.GetChannelName();
        auto msg = packet.GetMessage();
        for (auto& channel : mChannels) {
            if (channel->GetTitle() == channelTitle) {
                Model::MessagePtr message = std::make_shared<Model::Message>(mUsers[uid], msg);
                channel->AddMessage(message);
            }
        }
    }

}