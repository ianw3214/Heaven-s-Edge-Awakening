#include "lib.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include "../../libs/glew-2.1.0/src/glew.c"
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

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
        if (socket.receive(temp, response, sizeof(response)) > 0) {
            std::cout << response << '\n';
        }
    }
}

int main(int argc, char* argv[]) {

    GLuint programID;
    GLint gVertex2DLocation = -1;
    GLuint VBO = 0;
    GLuint VAO = 0;

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    socket_init();

    // setup openGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window * window = SDL_CreateWindow("Heaven's Edge: Awakening", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        std::cout << "Could not create openGL context, ERROR: " << SDL_GetError() << std::endl;
        return 1;
    } else {
        // initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            std::cout << "Could not initialize GLEW, ERROR: " << glewGetErrorString(glewError) << std::endl;
            return 1;
        }
        // use vsync I guess cause why not
        if (SDL_GL_SetSwapInterval(1) < 0) {
            std::cout << "Unable to set VSync, ERROR: " << SDL_GetError() << std::endl;
        }
    }

    Socket socket;
    if (!socket.open(INADDR_ANY)) {
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
    SDL_Quit();

    return 0;

}