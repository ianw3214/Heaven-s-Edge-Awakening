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

bool set_socket_nonblocking(int handle) {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    int nonBlocking = 1;
    if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
        std::cout << "Failed to set non-blocking\n";
        return false;
    }
    #elif PLATFORM == PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
    if (ioctlsocket(handle, FIONBIO, &nonBlocking) != 0) {
        std::cout << "Failed to set non-blocking\n";
        return false;
    }
    #endif
    return true;
}