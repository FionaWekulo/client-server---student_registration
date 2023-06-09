#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

struct Student {
    char name[50];
    int age;
};

void registerStudent(struct Student student) {
    FILE *file = fopen("students.txt", "a+");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fprintf(file, "Name: %s, Age: %d\n", student.name, student.age);
    fclose(file);
}

int main() {
    int sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pid_t pid;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(1);
    }

    printf("Server is listening on port 12345...\n");

    while (1) {
        addr_size = sizeof(client_addr);
        char buffer[BUFFER_SIZE];

        // Receive data from client
        ssize_t num_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size);
        buffer[num_bytes] = '\0';

        // Fork a child process to handle the client request
        pid = fork();
        if (pid == -1) {
            perror("Error forking process");
            exit(1);
        }

        if (pid == 0) {
            // Child process
            close(sockfd);

            // Parse student data
            struct Student student;
            sscanf(buffer, "%[^,],%d", student.name, &student.age);

            // Register student
            registerStudent(student);

            // Send response to client
            sendto(client_sockfd, "Student registered successfully!\n", strlen("Student registered successfully!\n"), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));

            close(client_sockfd);
            exit(0);
        } else {
            // Parent process
            close(client_sockfd);
        }
    }

    close(sockfd);
    return 0;
}
