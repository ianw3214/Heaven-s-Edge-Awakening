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