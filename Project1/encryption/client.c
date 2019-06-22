/*
	Operating System Project 1: Caesar Encryption Server
	Author: Zihan Wang    Student ID: 517021911179
	
	client.c
	
	Client program of the encryption service.
	
*/

#include "encrypt.h"
#include <netdb.h>

int main(int argc, char *argv[]) {
    /* Network Setup */
    // Open socket
    int portno = 2050;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Error opening socket.\n");
        exit(1);
    }
    
    // Find host
    struct hostent *server = gethostbyname("127.0.0.1");
    if (!server) {
        printf("Error. No such host.\n");
        exit(1);
    }
    
    // Connect to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error connecting socket.\n");
        exit(1);
    }
    printf("Connected to socket at %s.\n", time_str());

    /* Client Program */
    while (1) {
    	// Acquire input message from user
        char plain[BUFFER_SIZE];
        printf("Please enter the message: \n");
        gets(plain);
        write(sockfd, plain, strlen(plain) + 1);
        
        // Exit if user inputs ":q"
        if (strcmp(plain, ":q") == 0) {
            printf("Client closed at %s.\n", time_str());
            break;
        }
        
        // Read encrypted text from server
        char encrypted[BUFFER_SIZE];
        int len = read(sockfd, encrypted, BUFFER_SIZE);
        printf("From server at %s: %s\n", time_str(), encrypted);
    }

    return 0;
}
