ifeq ($(OS),Windows_NT)
DEPENDENCIES = -lwsock32 -I../libs/mingw_threads
CLIENT_TARGET = build/client.exe
SERVER_TARGET = build/server.exe
else
DEPENDENCIES = -pthread
CLIENT_TARGET = build/client
SERVER_TARGET = build/server
endif

ARGS = -std=c++17

INCLUDES = -I ../libs/SDL2-2.0.7/include			\
	-I ../libs/glew-2.1.0/include
LIBRARIES = -lopengl32 -lgdi32 -luser32 -lkernel32 	\
	-L ../libs/SDL2-2.0.7/lib						\
	-lSDL2main -lSDL2								\
	-lole32 -loleaut32 -lshell32 -lversion -luuid 	\
	-ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 	\
	-lwinmm -limm32

all: client server

client: src/client.cpp
	g++ src/client.cpp src/lib.cpp -o $(CLIENT_TARGET) -Wall $(DEPENDENCIES) $(ARGS) $(INCLUDES) $(LIBRARIES)

server: src/server.cpp
	g++ src/server.cpp src/lib.cpp -o $(SERVER_TARGET) -Wall $(DEPENDENCIES) $(ARGS)