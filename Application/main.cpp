#include "Connection.hpp"
#include "Packet.hpp"
#include "BackgroundWorker.hpp"
#include "ClientController.hpp"

int main(int argc, char** argv) {
    Application::Controller::ClientController controller;
    controller.Initialize();
    controller.Render();
    /*
    Application::Model::TcpCoonection connection;
    connection.Init(5000);
    connection.Connect();
    Utils::BackgroundWorker worker;
    worker.SetInterval(5000);
    worker.OnTick([&](void){

        Utils::ConnectPacket pack1("User" );
        auto pack1Data = pack1.Data();
        auto pack1Size = pack1.OutputDataSize();
        connection.Send(pack1Data, pack1Size);

        Utils::PacketRequest request(Utils::Signal::GUESTS);
        auto requestData = request.Data();
        auto requestSize = request.OutputDataSize();
        connection.Send(requestData, requestSize);


    });
    worker.Start();
    while (true) {
        connection.Receive();
    }
     */
    return 0;
}