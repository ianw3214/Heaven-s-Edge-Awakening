#include "lib.h"

#include <iostream>

int main(int argc, char* argv[]) {

    socket_init();

    Socket socket;
    if (!socket.open(9999)) {
        std::cout << "Failed to bind socket port\n";
        return 1;
    }

    while (true) {
        Address sender;
        unsigned char packet_data[256];

        socket.receive(sender, packet_data, sizeof(packet_data));

        std::cout << "CLIENT ADDRESS: " << sender.getAddress() << '\n';
        std::cout << "CLIENT PORT: " << sender.getPort() << '\n';

        // just print the packet for now
        std::cout << packet_data << '\n';

        // send a test data packet back
        char response[] = "Server response";
        socket.send(sender, response, sizeof(response));
    }

    socket_cleanup();

    return 0;

}