#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create a socket
    int c_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_socket == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }

    // Connect to the server
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(4444);
    // Replace "SERVER_IP_ADDRESS" with the IP address of the server computer
    inet_pton(AF_INET, "127.0.0.1", &s_address.sin_addr);
    if (connect(c_socket, (struct sockaddr*)&s_address, sizeof(s_address)) == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }

    // Send messages to the server
    char message[1024];
    char buffer[1024];
    while (1) {
        // Prompt the user to enter two numbers
        int num1, num2;
        printf("Enter any two numbers:\n");
        scanf("%d %d", &num1, &num2);

        // Prompt the user to select an operator
        char op;
                printf("------------SIMPLE CALCULATOR------------\n");
printf("Select an operation to perform:\n");
        printf("1. Addition \n");
        printf("2. Subtraction \n");
        printf("3. Multiplication \n");
        printf("4. Division \n");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                op = '+';
                break;
            case 2:
                op = '-';
                break;
            case 3:
                op = '*';
                break;
            case 4:
                op = '/';
                break;
            default:
                fprintf(stderr, "Invalid choice\n");
                continue;
        }

        // Send the message to the server
        sprintf(message, "%d %d %c\n", num1, num2, op);
        send(c_socket, message, strlen(message), 0);

        // Read the result from the server
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(c_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            fprintf(stderr, "Failed to receive data from server\n");
            break;
        } else if (bytes_received == 0) {
            printf("Server disconnected\n");
            break;
        }

        // Display the result
        int result = atoi(buffer);
        printf("Result: %d\n", result);

        // Prompt the user to perform another operation or exit
        printf("Choose next action:\n");
        printf("1. Perform another operation\n");
        printf("2. Exit\n");
        int option;
        scanf("%d", &option);
        if (option == 2) {
            break;
        }
    }

    // Close the socket
    close(c_socket);

    return 0;
}
