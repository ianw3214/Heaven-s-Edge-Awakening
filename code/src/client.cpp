#include "lib.h"

#include <iostream>

int main(int argc, char* argv[]) {

    socket_init();

    Socket socket;
    if (!socket.open(9999)) {
        std::cout << "Failed to bind socket port\n";
        return 1;
    }

    // 54.208.98.2
    Address server(54, 208, 98, 2, 9999);

    char packet_data[] = "test message\n";
    socket.send(server, packet_data, sizeof(packet_data));

    // receive a response from the server
    char response[256];
    Address temp;
    socket.receive(temp, response, sizeof(response));

    std::cout << "SERVER RESPONSE: " << response << '\n';

    socket_cleanup();

    return 0;

}