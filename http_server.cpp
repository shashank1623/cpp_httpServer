#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <csignal>

bool running = true;

// Signal handler for SIGINT (Ctrl + C)
void sigintHandler(int signal) {
    std::cout << "Stopping server..." << std::endl;
    running = false;
}

int main() {
    // Register signal handler for SIGINT
    signal(SIGINT, sigintHandler);

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Bind the socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Use port 8080
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket\n";
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Error listening on socket\n";
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";

    // Loop to handle connections
    while (running) {
        // Accept connections
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        // Receive and send a simple response
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World";
        send(clientSocket, response, strlen(response), 0);

        // Close the connection
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
