#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

struct info {
    char name[10];
    char reg[10];
    char serial[10];
};

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    struct info info;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Main menu
        int option;
        printf("\n1. Register a new user\n");
        printf("2. View existing registrations\n");
        printf("3. Exit\n");
        printf("Enter an option: ");
        scanf("%d", &option);

        switch(option) {
            case 1: // Register a new user
                send(sock, "register", sizeof("register"), 0);
                printf("Enter name: ");
                scanf("%s", info.name);
                printf("Enter serial number: ");
                scanf("%s", info.serial);
                printf("Enter registration number: ");
                scanf("%s", info.reg);

                send(sock, &info, sizeof(info), 0);
                read(sock, buffer, 1024);
                printf("%s\n",buffer);
                if(strcmp(buffer,"error")==0) {exit(1);}
                break;
            case 2: // View existing registrations
                send(sock, "view", strlen("view"), 0);

                valread = read(sock, buffer, 1024);
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

close(sock);
return 0;
}
