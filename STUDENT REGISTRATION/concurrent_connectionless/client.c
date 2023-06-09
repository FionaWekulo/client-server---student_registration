#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Get student information
    char name[50];
    int age;

    printf("Enter student name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';  // Remove newline character

    printf("Enter student age: ");
    scanf("%d", &age);
    getchar();  // Consume newline character

    // Format the student data as a string
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s,%d", name, age);

    // Send student data to server
    ssize_t num_bytes = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (num_bytes == -1) {
        perror("Error sending data");
        exit(1);
    }

    // Receive response from server
    char response[BUFFER_SIZE];
    num_bytes = recvfrom(sockfd, response, BUFFER_SIZE, 0, NULL, NULL);
    response[num_bytes] = '\0';

    printf("Server response: %s\n", response);

    close(sockfd);
    return 0;
}
