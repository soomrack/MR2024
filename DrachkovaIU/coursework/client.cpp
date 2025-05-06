all: server client

server:
    g++ server.cpp -o server -w