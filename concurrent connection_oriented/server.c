#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

struct ClientData {
    int client_socket;
    char* filename;
};

void* handle_client(void* arg) {
    struct ClientData* client_data = (struct ClientData*)arg;
    int client_socket = client_data->client_socket;
    char* filename = client_data->filename;
    char buffer[1024];
    char* line = NULL;
    size_t len = 0;

    // Handle multiple requests from this client
    while (1) {
        // Receive request from client
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            break;
        }

        // Check if the client wants to exit
        if (strncmp(buffer, "exit", bytes_received) == 0) {
            break;
        }

        // Process request
        buffer[bytes_received] = '\0';
        char* serial_number = buffer;
        int exists = 0;
        FILE* infile = fopen(filename, "r");
        while (getline(&line, &len, infile) != -1) {
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
            send(client_socket, "Serial number added.\n", 20, 0);
        } else {
            send(client_socket, "Serial number already exists.\n", 29, 0);
            continue;
        }

        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            break;
        }
        buffer[bytes_received] = '\0';
        char* registration_number = buffer;
        exists = 0;
        infile = fopen(filename, "r");
        while (getline(&line, &len, infile) != -1) {
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
            send(client_socket, "Registration number added.\n", 25, 0);
        } else {
            send(client_socket, "Registration number already exists.\n", 34, 0);
        }

        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from client\n");
            break;
        }
        buffer[bytes_received] = '\0';
        char* name = buffer;
        FILE* outfile = fopen(filename, "a");
        fprintf(outfile, "%s\n", name);
        fclose(outfile);
        send(client_socket, "Name added.\n", 11, 0);
    }

    // Close connection with client
    close(client_socket);

    free(client_data->filename);
    free(client_data);

    return NULL;
}

// Serial Number Program
int main() {
    char* filename = "concurrent_connection.txt";

    // Create a server-socket (listener) and bind it to a port
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
    }
struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_port = htons(8080);
server_address.sin_addr.s_addr = INADDR_ANY;

int bind_result = bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
if (bind_result == -1) {
    fprintf(stderr, "Failed to bind socket\n");
    exit(EXIT_FAILURE);
}

// Listen for connections from clients
int listen_result = listen(server_socket, 5);
if (listen_result == -1) {
    fprintf(stderr, "Failed to listen for connections\n");
    exit(EXIT_FAILURE);
}

printf("Waiting for clients...\n");

// Handle connections from clients
while (1) {
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
    if (client_socket == -1) {
        fprintf(stderr, "Failed to accept connection from client\n");
        continue;
    }

    printf("Client connected\n");

    // Spawn a new thread to handle this client
    pthread_t client_thread;
    struct ClientData* client_data = malloc(sizeof(struct ClientData));
    client_data->client_socket = client_socket;
    client_data->filename = strdup(filename);
    int thread_result = pthread_create(&client_thread, NULL, handle_client, (void*)client_data);
    if (thread_result != 0) {
        fprintf(stderr, "Failed to create thread for client\n");
        close(client_socket);
        free(client_data->filename);
        free(client_data);
    }
}

// Close server socket
close(server_socket);

return 0;
}
