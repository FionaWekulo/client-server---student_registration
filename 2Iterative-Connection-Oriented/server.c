#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    // Create a socket
    int s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (s_socket == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }

    // Bind the socket to an IP address and port
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(4444);
    s_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(s_socket, (struct sockaddr*)&s_address, sizeof(s_address)) == -1) {
        fprintf(stderr, "Failed to bind socket\n");
        return 1;
    }

    // Display the port the server is running on
    printf("Server running on port %d\n", ntohs(s_address.sin_port));

    // Listen for incoming connections
    if (listen(s_socket, 10) == -1) {
        fprintf(stderr, "Failed to listen on socket\n");
        return 1;
    }

    // Accept an incoming connection
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int c_socket = accept(s_socket, (struct sockaddr*)&client_address, &client_address_size);
    if (c_socket == -1) {
        fprintf(stderr, "Failed to accept connection\n");
        return 1;
    }

    // Log a message when a client connects
    printf("Client connected\n");

    // Read messages from the client
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(c_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            break;
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }

        // Parse the message
        int num1, num2;
        char op;
        sscanf(buffer, "%d %d %c", &num1, &num2, &op);

        // Perform the calculation
        int result;
        switch (op) {
            case '+':
                result = num1 + num2;
                break;
            case '-':
                result = num1 - num2;
                break;
            case '*':
                result = num1 * num2;
                break;
            case '/':
                result = num1 / num2;
                break;
            default:
                fprintf(stderr, "Invalid operator: %c\n", op);
                continue;
        }

        // Send the result back to the client
        char response[1024];
        sprintf(response, "%d\n", result);
        send(c_socket, response, strlen(response), 0);
    }

    // Close the sockets
    close(c_socket);
    close(s_socket);

    return 0;
}
