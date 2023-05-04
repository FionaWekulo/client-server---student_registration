#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int main() {
    // Create a socket and connect to the server
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Failed to create client socket\n");
        exit(EXIT_FAILURE);
    }

	//CONNECT TO THE SERVER
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }

    printf("Connected to server\n");

    char serial_number[1024];
    char registration_number[1024];
    char name[1024];

    // Loop until the user chooses to exit
    while (1) {
        printf("Enter a serial number (or 'exit' to quit): ");
        fgets(serial_number, 1024, stdin);

        // Check if the user wants to exit
        if (strcmp(serial_number, "exit\n") == 0) {
            send(client_socket, serial_number, strlen(serial_number), 0);
            break;
        }

        // Send the serial number to the server
        int bytes_sent = send(client_socket, serial_number, strlen(serial_number), 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "Failed to send data to server\n");
            continue;
        }

        // Wait for response from server
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from server\n");
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        if (strcmp(buffer, "Serial number already exists.\n") == 0) {
            continue;
        }

        while (1) {
            printf("Enter a Registration Number (P15/...): ");
            fgets(registration_number, 1024, stdin);

            // Send the registration number to the server
            bytes_sent = send(client_socket, registration_number, strlen(registration_number), 0);
            if (bytes_sent == -1) {
                fprintf(stderr, "Failed to send data to server\n");
                break;
            }

            // Wait for response from server
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received == -1) {
                fprintf(stderr, "Failed to receive data from server\n");
                break;
            }
            buffer[bytes_received] = '\0';
            printf("%s\n", buffer);
            if (strcmp(buffer, "Registration number already exists.\n") == 0) {
                continue;
            } else {
                break;
            }
        }

        printf("Enter a name: ");
        fgets(name, 1024, stdin);

        // Send the name to the server
        bytes_sent = send(client_socket, name, strlen(name), 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "Failed to send data to server\n");
            continue;
        }

        // Wait for response from server
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1)
{
fprintf(stderr, "Failed to receive data from server\n");
continue;
}
buffer[bytes_received] = '\0';
printf("%s\n", buffer);
}

// Close the socket
close(client_socket);
return 0;
}
