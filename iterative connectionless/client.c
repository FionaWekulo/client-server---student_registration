#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Serial Number Program
int main() {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Failed to create client socket\n");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    char serial_number[1024];
    char registration_number[1024];
    char name[1024];

    // Loop until the user chooses to exit
    while (1) {
        printf("Enter a serial number (or 'exit' to quit): ");
        fgets(serial_number, 1024, stdin);
        serial_number[strcspn(serial_number, "\n")] = 0;

        // Check if the user wants to exit
        if (strcmp(serial_number, "exit") == 0) {
            sendto(client_socket, serial_number, strlen(serial_number), 0, (struct sockaddr*)&server_address, sizeof(server_address));
            break;
        }

        // Send the serial number to the server
        int bytes_sent = sendto(client_socket, serial_number, strlen(serial_number), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        if (bytes_sent == -1) {
            fprintf(stderr, "Failed to send data to server\n");
            continue;
        }

        // Wait for response from server
        char buffer[1024];
        socklen_t server_address_size = sizeof(server_address);
        int bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &server_address_size);
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
            registration_number[strcspn(registration_number, "\n")] = 0;

            // Send the registration number to the server
            bytes_sent = sendto(client_socket, registration_number, strlen(registration_number), 0, (struct sockaddr*)&server_address, sizeof(server_address));
            if (bytes_sent == -1) {
                fprintf(stderr, "Failed to send data to server\n");
                break;
            }

            // Wait for response from server
            bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &server_address_size);
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
        name[strcspn(name, "\n")] = 0;

        // Send the name to the server
        bytes_sent = sendto(client_socket, name, strlen(name), 0, (struct sockaddr*)&server_address, sizeof(server_address));
        if (bytes_sent == -1) {
        fprintf(stderr, "Failed to send data to server\n");
        continue;
        }

            // Wait for response from server
    bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &server_address_size);
    if (bytes_received == -1) {
        fprintf(stderr, "Failed to receive data from server\n");
        continue;
    }
    buffer[bytes_received] = '\0';
    printf("%s\n", buffer);

    // Print the summary of the entered data
    printf("Summary:\nSerial Number: %s\nRegistration Number: %s\nName: %s\n", serial_number, registration_number, name);
}

// Close the socket
close(client_socket);

return 0;
}
