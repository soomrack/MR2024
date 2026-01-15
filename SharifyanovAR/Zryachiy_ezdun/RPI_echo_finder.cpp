#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void handleCommand(char command) {
    switch(command) {
        case 'w':
            std::cout << "Command: FORWARD" << std::endl;
            // Code for forward movement
            break;
        case 'a':
            std::cout << "Command: LEFT" << std::endl;
            // Code for left movement
            break;
        case 's':
            std::cout << "Command: BACKWARD" << std::endl;
            // Code for backward movement
            break;
        case 'd':
            std::cout << "Command: RIGHT" << std::endl;
            // Code for right movement
            break;
        case ' ':
            std::cout << "Command: STOP" << std::endl;
            // Code for stop
            break;
        case 'q':
            std::cout << "Shutting down..." << std::endl;
            running = false;
            break;
        default:
            std::cout << "Unknown command: " << command << std::endl;
    }
}

void handleClient(int client_socket) {
    char buffer[1024];
    
    std::cout << "Client connected!" << std::endl;
    std::cout << "Available commands: w, a, s, d, space, q(exit)" << std::endl;
    
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        
        // Read data from client
        int valread = read(client_socket, buffer, sizeof(buffer));
        
        if (valread > 0) {
            // Process each command in buffer
            for (int i = 0; i < valread; i++) {
                handleCommand(buffer[i]);
            }
        } else if (valread == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        } else {
            std::cerr << "Read error" << std::endl;
            break;
        }
    }
    
    close(client_socket);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "setsockopt error" << std::endl;
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
    address.sin_port = htons(8888); // Port 8888
    
    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind error" << std::endl;
        return -1;
    }
    
    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen error" << std::endl;
        return -1;
    }
    
    std::cout << "Server started. Waiting for connection on port 8888..." << std::endl;
    
    // Main server loop - accept multiple clients
    while (running) {
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (client_socket < 0) {
            std::cerr << "Accept error" << std::endl;
            continue;
        }
        
        // Handle client in a separate thread
        std::thread client_thread(handleClient, client_socket);
        client_thread.detach(); // Don't wait for thread to finish
    }
    
    // Close server socket
    close(server_fd);
    
    std::cout << "Server stopped" << std::endl;
    return 0;
}
