#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024
struct info
{
    char name[10];
    char reg[10];
    char serial[10];
};

int main() {
    int sockfd,valread;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct info info;
        int option;
        printf("\n1. Register a new user\n");
        printf("2. View existing registrations\n");
        printf("3. Exit\n");
        printf("Enter an option: ");
        scanf("%d", &option);

        switch(option) {
            case 1: // Register a new user
                sendto(sockfd, "register", strlen("register"), 0,(const struct sockaddr *)&serverAddr, sizeof(serverAddr));
                printf("Enter name: ");
                scanf("%s", info.name);
                printf("Enter serial number: ");
                scanf("%s", info.serial);
                printf("Enter registration number: ");
                scanf("%s", info.reg);

                sendto(sockfd, &info, sizeof(info), 0,(const struct sockaddr *)&serverAddr, sizeof(serverAddr));

                recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
                printf("%s\n",buffer);
                if(strcmp(buffer,"error")==0) {exit(1);}
                break;
            case 2: // View existing registrations
                sendto(sockfd,"view", strlen("view"), 0,(const struct sockaddr *)&serverAddr, sizeof(serverAddr));
                recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);
                
                printf("Registrations:\n");
                printf("%s",buffer);
                
                break;
            
            case 3: // Exit
                printf("Exiting program\n");
                break;
            default:
                printf("Invalid option\n");
                break;
        }
    }

    return 0;
}