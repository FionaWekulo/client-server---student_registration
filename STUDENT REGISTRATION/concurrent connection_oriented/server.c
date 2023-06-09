#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

typedef struct {
    char name[20];
    char serial[10];
char reg[10];
} Student;

void viewStudent(int sock){

	printf("Viewing........\n");
        // Send the contents of the file to the client
        FILE *database = fopen("students.txt", "a+");
        if (database == NULL)
        {
            send(sock, "Error openning database", strlen("Error openning database"), 0);
        }
        else
        {
            char line[1024];
            char result[1024];
            char temp[1024];
            while(fgets(temp,50,database)){
            	strcpy(line,temp);
        
            	strcat(result,line);
            }
            send(sock, result, strlen(result), 0);
            strcpy(result,"");
            fclose(database);
            printf("Done sending!\n");
        }

}

void registerStudent(int clientSocket) {
    Student student;

    // Receive student data from client
    if (recv(clientSocket, &student, sizeof(Student), 0) < 0) {
        perror("Failed to receive student data");
        return;
    }

    printf("Received student data:\n");
    printf("Name: %s\n", student.name);
    printf("Serial: %s\n", student.serial);
	printf("Reg: %s\n", student.reg);

    // Store student data in a file
    FILE *file = fopen("students.txt", "a+");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
	char temp[100];
        int index = 0;
        
        while(fgets(temp,100,file)!=NULL){
        	char *token = strtok(temp," ");
        	int count = 0;
        	while(token != NULL){
        	
        		if (count== 2){
        			if(strstr(token,student.reg)!=NULL){
				index++;
				
        			}
        		}
        		count++;
        		token = strtok(NULL, " ");
        	}
        	
        }
        if (index == 0)
        {
            fprintf(file, "%s %s %s\n", student.name,student.serial,student.reg);

    fclose(file);

    // Send registration confirmation to client
    char response[] = "Registration successful!";
    if (send(clientSocket, response, sizeof(response), 0) < 0) {
        perror("Failed to send registration confirmation");
        return;
    }

    printf("Registration successful!\n");
        }else{
        	send(clientSocket, "Registration number in use!", strlen("Registration number in use!"), 0);
        	printf("Registration failed!!");
        }

    
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;
    pid_t childPid;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);

    // Bind socket to the specified IP and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

    while (1) {
        // Accept client connection
        clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the client
        childPid = fork();
        if (childPid < 0) {
            perror("Failed to create child process");
            exit(EXIT_FAILURE);
        }

        if (childPid == 0) {
            // Child process (handle client)
	char m[1024];
	recv(clientSocket, m, sizeof(m), 0);
            close(serverSocket);  // Close server socket in child process

            if(strcmp(m, "register") == 0){registerStudent(clientSocket);}else if (strcmp(m, "view") == 0){viewStudent(clientSocket);}

            close(clientSocket);  // Close client socket in child process
            exit(EXIT_SUCCESS);
        } else {
            // Parent process (continue listening for new connections)
            close(clientSocket);  // Close client socket in parent process
        }
    }

    close(serverSocket);
    return 0;
}
