#include "Connection.hpp"
#include "Packet.hpp"
#include "Connection.hpp"

int main(int argc, char** argv) {
    Application::TcpCoonection connection;
    connection.Initialize(5000);
    while (true) {
        connection.Send(std::make_shared<Utils::ConnectPacket>(Utils::InputMemory(nullptr)));
    }
    system("pause");
    return 0;
}