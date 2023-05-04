#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Serial Number Program
int main() {
    char* filename = "iterative_connectionless-C.txt";
    char serial_number[1024];
    char registration_number[1024];
    char name[1024];

    // Create a server-socket (listener) and bind it to a port (higher than 1024)
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Failed to create server socket\n");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Failed to bind server socket\n");
        return 1;
    }

    printf("Server listening on port 3000\n");

    while (true) {
        // Receive request from client
        char buffer[1024];
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            continue;
        }

        // Check if the client wants to exit
        if (strncmp(buffer, "exit", bytes_received) == 0) {
            break;
        }

        // Process request
        strncpy(serial_number, buffer, bytes_received);
        serial_number[bytes_received] = '\0';
        bool exists = false;
        FILE* infile = fopen(filename, "a+");
        char line[1024];
        while (fgets(line, sizeof(line), infile)) {
            if (strstr(line, serial_number) != NULL) {
                exists = true;
                break;
            }
        }
        fclose(infile);
        if (!exists) {
            FILE* outfile = fopen(filename, "a+");
            fprintf(outfile, "%s\n", serial_number);
            fclose(outfile);
            sendto(server_socket, "Serial number added.\n", 20, 0, (struct sockaddr*)&client_address, client_address_size);
        } else {
            sendto(server_socket, "Serial number already exists.\n", 29, 0, (struct sockaddr*)&client_address, client_address_size);
            continue;
        }

        bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            continue;
        }
        strncpy(registration_number, buffer, bytes_received);
        registration_number[bytes_received] = '\0';
        exists = false;
        infile = fopen(filename, "a+");
        while (fgets(line, sizeof(line), infile)) {
            if (strstr(line, registration_number) != NULL) {
                exists = true;
                break;
            }
        }
        fclose(infile);
        if (!exists) {
            FILE* outfile = fopen(filename, "a+");
            fprintf(outfile, "%s\n", registration_number);
            fclose(outfile);
            sendto(server_socket, "Registration number added.\n", 25, 0, (struct
            sockaddr*)&client_address, client_address_size);
        } else {
        sendto(server_socket, "Registration number already exists.\n", 34, 0, (struct sockaddr*)&client_address, client_address_size);
        continue;
        }
            bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
    if (bytes_received == -1) {
        fprintf(stderr, "Failed to receive data from client\n");
        continue;
    }
    strncpy(name, buffer, bytes_received);
    name[bytes_received] = '\0';

    FILE* outfile = fopen(filename, "a+");
    fprintf(outfile, "%s\n", name);
    fclose(outfile);

    // Send confirmation message to client
    sendto(server_socket, "Name added.\n", 12, 0, (struct sockaddr*)&client_address, client_address_size);
}

// Close server socket
close(server_socket);

return 0;
}
