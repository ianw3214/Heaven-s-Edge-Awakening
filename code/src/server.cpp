#include "lib.h"

#include <iostream>
#include <string>

#if PLATFORM == PLATFORM_WINDOWS
#include "mingw.thread.h"
#else
#include <thread>
#endif

#define MAX_CLIENTS 10

// global variables to keep track of clients
Address clients[MAX_CLIENTS];

// helper function to add clients to the client list
bool add_client(Address client) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].getAddress() == 0) {
            clients[i] = client;
            return true;
        }
    }
    return false;
}

// send the message from the sender to all other clients
void send_message(Socket socket, char message[], Address sender) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].getAddress() == sender.getAddress() && clients[i].getPort() == sender.getPort()) {
            break;
        }
        if (clients[i].getAddress() != 0) {
            socket.send(sender, message, 256);
        }
    }
}

// thread to receive incoming packets
void receive() {

    Socket socket;
    if (!socket.open(9999)) {
        std::cout << "Failed to bind socket port\n";
        return;
    }

    while (true) {
        Address sender;
        unsigned char packet_data[256];

        if (socket.receive(sender, packet_data, sizeof(packet_data)) > 0) {
            // print out the message from the client
            std::cout << packet_data << "\n---\n";

            // if the client is not already stored, add the client to the list
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].getAddress() != sender.getAddress() || clients[i].getPort() != sender.getPort()) {
                    add_client(sender);
                }
            }

            // then send the received message to the clients
            send_message(socket, (char*) packet_data, sender);
        }
    }
}

int main(int argc, char* argv[]) {

    socket_init();

    std::thread receiveThread(receive);
    receiveThread.detach();

    while(true) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "quit") {
            break;
        }
    }

    socket_cleanup();

    return 0;

}