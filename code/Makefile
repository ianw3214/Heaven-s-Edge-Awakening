ifeq ($(OS),Windows_NT)
DEPENDENCIES = -lwsock32 -I../libs/mingw_threads
CLIENT_TARGET = build/client.exe
SERVER_TARGET = build/server.exe
else
CLIENT_TARGET = build/client
SERVER_TARGET = build/server
endif

ARGS = -std=c++17

all: client server

client: src/client.cpp
	g++ src/client.cpp src/lib.cpp -o $(CLIENT_TARGET) -Wall $(DEPENDENCIES) $(ARGS)

server: src/server.cpp
	g++ src/server.cpp src/lib.cpp -o $(SERVER_TARGET) -Wall $(DEPENDENCIES) $(ARGS)