#include <iostream>

#include "lib.h"

// function to initalize socket libraries
bool socket_init() {
    #if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        return WSAStartup( MAKEWORD(2, 2), &WsaData) == NO_ERROR;
    #else
        return true;
    #endif
}

// function to cleanup socket libraries
void socket_cleanup() {
    #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
    #endif
}

Address::Address() : address(0), port(0) {
    // default to 0:0:0:0 @ port 0
}

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) {
    this->address = (a << 24) | (b << 16) | (c << 8) | d;
    this->port = port;
}

Address::Address(unsigned int address, unsigned short port) {
    this->address = address;
    this->port = port;
}

Address::~Address() {
    // do nothing I guess
}

bool Address::operator==(const Address& other) const {
    return address == other.address && port == other.port;
}

unsigned int Address::getAddress() const {
    return address;
}

unsigned char Address::getA() const {
    return ((0b11111111 << 24) & address) >> 24;
}
unsigned char Address::getB() const {
    return ((0b11111111 << 16) & address) >> 16;
}

unsigned char Address::getC() const {
    return ((0b11111111 << 8) & address) >> 8;
}
unsigned char Address::getD() const {
    return 0b11111111 & address;
}

unsigned short Address::getPort() const {
    return port;
}

Socket::Socket() {
    handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (handle <= 0) {
        // TODO: handle this error somehow
    }
}

Socket::~Socket() {
    if (is_open()) terminate();
}

bool Socket::open(unsigned short port) {
    // bind the socket to a port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) port);

    if (bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
        return false;
    }

    socket_open = true;
    return true;
}

bool Socket::set_nonblocking() {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int nonBlocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
        return false;
    }
    #elif PLATFORM == PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
        return false;
    }
    #endif
    return true;
}

void Socket::terminate() {
    #if PLATFORM == PLATFORM_WINDOWS
    closesocket(handle);
    #elif PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_MAC
    close(handle);
    #endif
    socket_open = false;
}

bool Socket::is_open() const {
    return socket_open;
}

bool Socket::send(const Address & destination, const void * data, int size) const {
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(destination.getAddress());
    dest.sin_port = htons(destination.getPort());
    int sent_bytes = sendto(handle, (const char*) data, size, 0, (sockaddr*)&dest, sizeof(sockaddr_in));
    if (sent_bytes != size) {
        return false;
    }
    return true;
}

int Socket::receive(Address & sender, void * data, int size) {
    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    sockaddr_in from;
    socklen_t fromLength = sizeof(from);

    int bytes = recvfrom(handle, (char*)data, size, 0, (sockaddr*)&from, &fromLength);
    if (bytes <= 0) return -1;

    unsigned int from_address = ntohl(from.sin_addr.s_addr);
    unsigned int from_port = ntohs(from.sin_port);
    sender = Address(from_address, from_port);

    return bytes;
}