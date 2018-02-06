#include "lib.h"

#include <iostream>

int main(int argc, char* argv[]) {

    socket_init();

    // create the UDP socket
    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handle <= 0) {
        std::cout << "Failed to create socket...\n";
        return 1;
    }

    // bind the socket to a port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) 9999);

    if (bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
        std::cout << "Failed to bind socket...\n";
        return 1;
    }

    while (true) {
        unsigned char packet_data[256];

        unsigned int max_packet_size = sizeof(packet_data);

        #if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
        #endif

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        int bytes = recvfrom(handle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);
        if (bytes <= 0) break;

        unsigned int from_address = ntohl(from.sin_addr.s_addr);
        unsigned int from_port = ntohs(from.sin_port);
        std::cout << "CLIENT ADDRESS: " << from_address << '\n';
        std::cout << "CLIENT PORT: " << from_port << '\n';

        // just print the packet for now
        std::cout << packet_data << '\n';
    }

    #if PLATFORM == PLATFORM_WINDOWS
    closesocket(handle);
    #elif PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_MAC
    close(handle);
    #endif

    socket_cleanup();

    return 0;

}