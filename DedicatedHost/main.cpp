#include "Server.hpp"
#include "Packet.hpp"

int main(int argc, char** argv) {
    auto server = new DedicatedHost::Server();
    server->Initialize("5000");
    server->Launch();
    system("pause");
    return 0;
}