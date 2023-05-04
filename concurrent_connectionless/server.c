#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

struct ClientData {
    int server_socket;
    char* filename;
    struct sockaddr_in client_address;
    socklen_t client_address_size;
};

void* handle_client(void* arg) {
    struct ClientData* client_data = (struct ClientData*)arg;
    int server_socket = client_data->server_socket;
    char* filename = client_data->filename;
    struct sockaddr_in client_address = client_data->client_address;
    socklen_t client_address_size = client_data->client_address_size;

    char serial_number[1024];
    char registration_number[1024];
    char name[1024];

    // Handle multiple requests from this client
    while (1) {
        // Receive request from client
        char buffer[1024];
        int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            break;
        }

        // Print received data
        printf("Received data from client: %.*s\n", bytes_received, buffer);

        // Check if the client wants to exit
        if (strncmp(buffer, "exit", bytes_received) == 0) {
            break;
        }

        // Process request
        strncpy(serial_number, buffer, bytes_received);
        serial_number[bytes_received] = '\0';
        int exists = 0;
        FILE* infile = fopen(filename, "r");
        char line[1024];
        while (fgets(line, sizeof(line), infile) != NULL) {
            if (strcmp(line, serial_number) == 0) {
                exists = 1;
                break;
            }
        }
        fclose(infile);
        if (!exists) {
            FILE* outfile = fopen(filename, "a");
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
            break;
        }

        // Print received data
        printf("Received data from client: %.*s\n", bytes_received, buffer);

        strncpy(registration_number, buffer, bytes_received);
        registration_number[bytes_received] = '\0';
        exists = 0;
        infile = fopen(filename, "r");
        while (fgets(line, sizeof(line), infile) != NULL) {
            if (strcmp(line, registration_number) == 0) {
                exists = 1;
                break;
            }
        }
        fclose(infile);
        if (!exists) {
            FILE* outfile = fopen(filename, "a");
            fprintf(outfile, "%s\n", registration_number);
            fclose(outfile);
            sendto(server_socket, "Registration number added.\n", 25, 0, (struct sockaddr*)&client_address, client_address_size);
        } else {
            sendto(server_socket, "Registration number already exists.\n", 34,0, (struct sockaddr*)&client_address, client_address_size);
continue;
}
    bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
    if (bytes_received == -1) {
        fprintf(stderr, "Failed to receive data from client\n");
        break;
    }

    // Print received data
    printf("Received data from client: %.*s\n", bytes_received, buffer);

    strncpy(name, buffer, bytes_received);
    name[bytes_received] = '\0';
    exists = 0;
    infile = fopen(filename, "r");
    while (fgets(line, sizeof(line), infile) != NULL) {
        if (strcmp(line, name) == 0) {
            exists = 1;
            break;
        }
    }
    fclose(infile);
    if (!exists) {
        FILE* outfile = fopen(filename, "a");
        fprintf(outfile, "%s\n", name);
        fclose(outfile);
        sendto(server_socket, "Name added.\n", 12, 0, (struct sockaddr*)&client_address, client_address_size);
    } else {
        sendto(server_socket, "Name already exists.\n", 21, 0, (struct sockaddr*)&client_address, client_address_size);
        continue;
    }
}

printf("Closing connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
close(server_socket);
free(filename);
free(client_data);
pthread_exit(NULL);
}

int main(int argc, char** argv) {
if (argc != 2) {
fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
exit(EXIT_FAILURE);
}
// Create server socket
int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
if (server_socket == -1) {
    fprintf(stderr, "Failed to create server socket\n");
    exit(EXIT_FAILURE);
}

// Bind server socket to port
struct sockaddr_in server_address;
memset(&server_address, 0, sizeof(server_address));
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(12345);
if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    fprintf(stderr, "Failed to bind server socket to port\n");
    exit(EXIT_FAILURE);
}

printf("Server listening on port %d\n", ntohs(server_address.sin_port));

// Start accepting clients
while (1) {
    // Receive request from client
    char buffer[1024];
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
    if (bytes_received == -1) {
        fprintf(stderr, "Failed to receive data from client\n");
        continue;
    }

    // Print received data
    printf("Received data from client: %.*s\n", bytes_received, buffer);

    // Check if the client wants to exit
    if (strncmp(buffer, "exit", bytes_received) == 0) {
        break;
    }

    // Create client data struct
    struct ClientData* client_data = (struct ClientData*)malloc(sizeof(struct ClientData));
    client_data->server_socket = server_socket;
    client_data->filename = strdup(argv[1]);
    client_data->client_address = client_address;
    client_data->client_address_size = client_address_size;
    struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_port = htons(9001);
server_address.sin_addr.s_addr = INADDR_ANY;
if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    fprintf(stderr, "Failed to bind server socket to port\n");
    exit(EXIT_FAILURE);
}

// Get filename from command line arguments
char* filename = strdup(argv[1]);
if (filename == NULL) {
    fprintf(stderr, "Failed to allocate memory for filename\n");
    exit(EXIT_FAILURE);
}

// Start handling clients
while (1) {
    // Accept connection from client
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    char buffer[1024];
    int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_address, &client_address_size);
    if (bytes_received == -1) {
        fprintf(stderr, "Failed to receive data from client\n");
        continue;
    }

    // Print received data
    printf("Received data from client: %.*s\n", bytes_received, buffer);

    // Create new thread to handle client
    pthread_t thread_id;
    struct ClientData* client_data = (struct ClientData*)malloc(sizeof(struct ClientData));
    if (client_data == NULL) {
        fprintf(stderr, "Failed to allocate memory for client data\n");
        continue;
    }
    client_data->server_socket = server_socket;
    client_data->filename = filename;
    client_data->client_address = client_address;
    client_data->client_address_size = client_address_size;
    int ret = pthread_create(&thread_id, NULL, handle_client, (void*)client_data);
    if (ret != 0) {
        fprintf(stderr, "Failed to create thread\n");
        free(client_data);
        continue;
    }
    printf("Created thread for %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
}
}
return 0;
}
/*
Sample output:

Received data from client: serial 1234
Created thread for 127.0.0.1:57284
Received data from client: registration 5678
Received data from client: name John Smith
Closing connection from 127.0.0.1:57284
*/
