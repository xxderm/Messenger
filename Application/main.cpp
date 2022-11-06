#include "Connection.hpp"
#include "Packet.hpp"
#include "Connection.hpp"
#include "BackgroundWorker.hpp"

int main(int argc, char** argv) {
    Application::TcpCoonection connection;
    connection.Init(5000);
    Utils::BackgroundWorker worker;
    worker.SetInterval(5000);
    worker.OnTick([&](void){
        char dataSend[DEFAULT_BUFFER_LEN] = "Hallo";
        auto isConn = connection.Connect();
        if (isConn) {
            std::cout << "Stop timer\n";
            worker.Stop();
        }
        connection.Send(dataSend, 6);
        connection.Disconnect();
    });
    worker.Start();
    while (true) {

    }
    system("pause");
    return 0;
}