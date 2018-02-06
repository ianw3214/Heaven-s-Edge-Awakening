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

    // 54.208.98.2
    unsigned int a = 54;
    unsigned int b = 208;
    unsigned int c = 98;
    unsigned int d = 2;

    unsigned int addr = (a << 24) | (b << 16) | (c << 8) | d;

    // bind the socket to a port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(addr);
    address.sin_port = htons((unsigned short) 9999);

    if (bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
        std::cout << "Failed to bind socket...\n";
        return 1;
    }

    set_socket_nonblocking(handle);

    char packet_data[128] = {'t', 'e', 's', 't', '\0'};
    int packet_size = sizeof(packet_data);
    int sent_bytes = sendto(handle, (const char*) packet_data, packet_size, 0, (sockaddr*)&address, sizeof(sockaddr_in));
    if (sent_bytes != packet_size) {
        std::cout << "Failed to send packet...\n";
        return 1;
    }

    #if PLATFORM == PLATFORM_WINDOWS
    closesocket(handle);
    #elif PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_MAC
    close(handle);
    #endif

    socket_cleanup();

    return 0;

}