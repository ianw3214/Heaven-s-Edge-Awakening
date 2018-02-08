// platform detection
#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
#include <winsock2.h>
#elif PLATFORM == PLATFORM_MAC || PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

bool socket_init();
void socket_cleanup();

bool set_socket_nonblocking(int handle);

class Address {
public:
    Address();
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
    Address(unsigned int address, unsigned short port);
    ~Address();

    bool operator==(const Address& other) const;

    unsigned int getAddress() const;
    unsigned char getA() const;
    unsigned char getB() const;
    unsigned char getC() const;
    unsigned char getD() const;
    unsigned short getPort() const;

private:
    unsigned int address;
    unsigned short port;

};

class Socket {
public:
    Socket();
    ~Socket();

    bool open(unsigned short port);
    void terminate();

    bool is_open() const;
    bool set_nonblocking();

    bool send(const Address & destination, const void * data, int size) const;
    int receive(Address & sender, void * data, int size);

private:
    bool socket_open;
    int handle;
    
};