#include "lib.h"

#include <iostream>
#include <string>

#if PLATFORM == PLATFORM_WINDOWS
#include "mingw.thread.h"
#else
#include <thread>
#endif

void receive(Socket socket) {
    while (true) {
        // receive a response from the server
        char response[256];
        Address temp;
        socket.receive(temp, response, sizeof(response));

        std::cout << response << '\n';
    }
}

int main(int argc, char* argv[]) {

    socket_init();

    Socket socket;
    if (!socket.open(9999)) {
        std::cout << "Failed to bind socket port\n";
        return 1;
    }

    // 54.208.98.2
    Address server(54, 208, 98, 2, 9999);

    char packet_data[] = "init message\n";
    socket.send(server, packet_data, sizeof(packet_data));

    std::thread receiveThread(receive, socket);
    receiveThread.detach();

    while(true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "quit") {
            break;
        } else {
            socket.send(server, input.c_str(), input.size() + 1);
        }
    }

    socket_cleanup();

    return 0;

}