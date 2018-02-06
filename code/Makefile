ifeq ($(OS),Windows_NT)
DEPENDENCIES = -lwsock32
CLIENT_TARGET = build/client.exe
SERVER_TARGET = build/server.exe
else
CLIENT_TARGET = build/client
SERVER_TARGET = build/server
endif

client: src/client.cpp
	g++ src/client.cpp src/lib.cpp -o $(CLIENT_TARGET) -Wall $(DEPENDENCIES)

server: src/server.cpp
	g++ src/server.cpp src/lib.cpp -o $(SERVER_TARGET) -Wall $(DEPENDENCIES)