#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    char name[20];
    char serial[10];
	char reg[10];
} Student;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;
    Student student;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);

    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        perror("Failed to configure server address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }
	printf("What would you like to do\n1.Register.\n2.View.\n");
	char y[BUFFER_SIZE];
	scanf("%s", y);
	getchar();
	send(clientSocket, y, sizeof(y), 0);

    if(strcmp(y, "register") == 0){
		// Get student details from user
    printf("Enter student name: ");
    fgets(student.name, sizeof(student.name), stdin);
    student.name[strcspn(student.name, "\n")] = '\0';
    printf("Enter student registration: ");
    fgets(student.reg, sizeof(student.reg), stdin);
    student.reg[strcspn(student.reg, "\n")] = '\0';
    printf("Enter student serial: ");
    fgets(student.serial, sizeof(student.serial), stdin);
    student.serial[strcspn(student.serial, "\n")] = '\0';

    // Send student data to server
    if (send(clientSocket, &student, sizeof(Student), 0) < 0) {
        perror("Failed to send student data");
        exit(EXIT_FAILURE);
    }
	}else if (strcmp(y, "register") != 0 && strcmp(y, "view") != 0){
	

	exit(EXIT_FAILURE);
}else{
	char response[BUFFER_SIZE];
	if (recv(clientSocket, response, sizeof(response), 0) < 0) {
        perror("Failed to receive registration confirmation");
        exit(EXIT_FAILURE);
    }

    printf("Data:\n%s", response);


}

    // Receive registration confirmation from server
    char response[BUFFER_SIZE];
    if (recv(clientSocket, response, sizeof(response), 0) < 0) {
        perror("Failed to receive registration confirmation");
        exit(EXIT_FAILURE);
    }

    close(clientSocket);
    return 0;
}
