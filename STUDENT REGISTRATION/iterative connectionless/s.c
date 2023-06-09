#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

struct sockaddr_in serverAddr, clientAddr;
void handle_request(int sock);

static FILE *database;
struct info
{
    char name[10];
    char reg[10];
    char serial[10];
};

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // Bind socket to server address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started and listening on port %d\n", PORT);

    while (1)
    {
        handle_request(sockfd);
    }
    return 0;
}

void handle_request(int sockfd)
{
    socklen_t len = sizeof(clientAddr);
    int valread;
    char buffer[1024] = {0};
    recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &len);

    if (strcmp(buffer, "view") == 0)
    {
        printf("Viewing........\n");
        // Send the contents of the file to the client
        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            sendto(sockfd, "Error opening database", strlen("Error opening database"), 0, (const struct sockaddr *)&clientAddr, len);

        }
        else
        {
            char line[1024];
            char result[1024];
            char temp[1024];
            while (fgets(temp, 50, database))
            {
                strcpy(line, temp);
                strcat(line, "\n");
                strcat(result, line);
            }
            sendto(sockfd, result, sizeof(buffer), 0,(const struct sockaddr *)&clientAddr, len);
            strcpy(result, "");
            fclose(database);
            printf("Done sending!\n");
        }
    }
    else if (strcmp(buffer, "register") == 0)
    {
        // Receive the registration data from the client and save it to the file
        struct info info;
        printf("Registering........\n");
        recvfrom(sockfd, &info, sizeof(info), 0, (struct sockaddr *)&clientAddr, &len);

        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            sendto(sockfd, "Error opening database", strlen("Error opening database"), 0, (const struct sockaddr *)&clientAddr, len);
        }
        char temp[100];
        int index = 0;

        while (fgets(temp, 100, database) != NULL)
        {
            char *token = strtok(temp, " ");
            int count = 0;
            while (token != NULL)
            {

                if (count == 2)
                {
                    printf("3: %s\n", token);
                    if (strstr(token, info.reg) != NULL)
                    {
                        index++;
                        printf("%d", index);
                    }
                }
                count++;
                token = strtok(NULL, " ");
            }
        }
        if (index == 0)
        {
            fprintf(database, "%s %s %s\n", info.name, info.serial, info.reg);
            fclose(database);
            sendto(sockfd, "Registration complete!", strlen("Registration complete!"), 0, (const struct sockaddr *)&clientAddr, len);

            printf("Done registering\n");
        }
        else
        {
            sendto(sockfd, "Registration number in use!", strlen("Registration number in use!"), 0, (const struct sockaddr *)&clientAddr, len);

            printf("Registration failed!!");
        }
    }
    else
    {
        sendto(sockfd, "unknown command", strlen("unknown command"), 0, (const struct sockaddr *)&clientAddr, len);

    }
}

