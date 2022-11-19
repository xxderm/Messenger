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
        mServer->mPacketView.AddInPacket(signal);
        switch (signal) {
            case Utils::CONNECT:
                this->AcceptNewUser(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::CONNECT);
                this->SendChannels();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNELS);
                this->SendGuests();
                mServer->mPacketView.AddOutPacket(Utils::Signal::GUESTS);
                break;
            case Utils::GUESTS:
                this->SendGuests();
                mServer->mPacketView.AddOutPacket(Utils::Signal::GUESTS);
                break;
            case Utils::CHANNELS:
                this->SendChannels();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNELS);
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
            case Utils::NEW_CHANNEL:
                this->AcceptNewChannel(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::NEW_CHANNEL);
                SendChannels();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNELS);
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
            case Utils::CLOSE_CHANNEL:
                this->AcceptCloseChannel(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::CLOSE_CHANNEL);
                this->SendChannels();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNELS);
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
            case Utils::CHANNEL_INFO_UPDATE:
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
            case Utils::GUEST_INFO_UPDATE:
                this->GuestInfoUpdate(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::GUEST_INFO_UPDATE);
                break;
            case Utils::PERSONAL_MESSAGE:
                this->SendPM(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::PERSONAL_MESSAGE);
                break;
            case Utils::CONNECT_CHANNEL:
                this->ConnectToChannel(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::CONNECT_CHANNEL);
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
            case Utils::DISCONNECT:
                this->AcceptCloseUser(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::DISCONNECT);
                break;
            case Utils::CHANNEL_LEAVE:
                this->DisconnectFromChannel(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_LEAVE);
                break;
            case Utils::CHANNEL_MESSAGE:
                this->ChannelMessage(input);
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_MESSAGE);
                this->SendChannelInfo();
                mServer->mPacketView.AddOutPacket(Utils::Signal::CHANNEL_INFO_UPDATE);
                break;
        }
    }

    void ServerController::SendGuests() {
        std::vector<std::shared_ptr<Utils::GuestProperties>> guests;
        if (mUsers.empty())
            return;
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
        if (mChannels.empty())
            return;
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
        mUsers[index]->SetId(index);
        auto newPacket = Utils::ConnectPacket(mUsers[index]->GetName(), index);
        mServer->SendTo(index, std::make_shared<Utils::ConnectPacket>(newPacket));
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
        auto msg = packet.GetMessageStr();
        for (auto& channel : mChannels) {
            if (channel->GetTitle() == channelTitle) {
                Model::MessagePtr message = std::make_shared<Model::Message>(mUsers[uid], msg);
                channel->AddMessage(message);
            }
        }
    }

    void ServerController::SendChannelInfo() {
        for (auto& channel : mChannels) {
            std::shared_ptr<Utils::ChannelInfoProperties> prop = std::make_shared<Utils::ChannelInfoProperties>();
            prop->AdminId = channel->GetAdminId();
            prop->Title = channel->GetTitle();
            auto messages = channel->GetMessages();
            auto users = channel->GetUsers();
            for (auto& msg : messages) {
                auto user = msg->GetUser();
                auto content = msg->GetContent();
                Utils::MessageProperties messageProperties;
                messageProperties.Message = content;
                messageProperties.Id = user->GetId();
                prop->Messages.push_back(messageProperties);
            }
            for (auto& user : users) {
                Utils::UserProperties userProperties;
                auto uid = user.second->GetId();
                auto name = user.second->GetName();
                userProperties.Id = uid;
                userProperties.Name = name;
                prop->Users.push_back(userProperties);
            }
            std::shared_ptr<Utils::Packet> packet = std::make_shared<Utils::ChannelInfoUpdatePacket>(prop);
            mServer->Send(packet);
        }
    }

}