/*
	Operating System Project 1: Caesar Encryption Server
	Author: Zihan Wang    Student ID: 517021911179
	
	server.c
	
	Multithreaded server program of the encryption service.
	
*/

#include "encrypt.h"
#include <pthread.h>
#include <semaphore.h>

#define ENCRYPT_OFFSET 3

/* Use Caesar Encryption to encrypt single character */
static char encrypt_char(char src) {
    if (src >= 'a' && src <= 'z') {
        src += ENCRYPT_OFFSET;
        if (src > 'z')
            src -= 26;
    } else if (src >= 'A' && src <= 'Z') {
        src += ENCRYPT_OFFSET;
        if (src > 'Z')
            src -= 26;
    }
    return src;
} 

/* Semaphore used to indicate the availability of server threads */
static sem_t service;

/* Server thread function that carries out the encryption service */
void *serve(void *param) {
    // Get socket file descriptor of current client
    int client_fd = *((int *) param);
    printf("Server thread initialized at %s.\n", time_str());
    
    while (1) {
        // Read from socket
        char plain[BUFFER_SIZE];
        int len = read(client_fd, plain, BUFFER_SIZE);
        
        // Exit if user types ":q"
        if (strcmp(plain, ":q") == 0) {
            sem_post(&service); // inform the main thread of available service
            printf("Server thread closed at %s.\n", time_str());
            pthread_exit(NULL);
        }
        printf("Message received at %s: %s\n", time_str(), plain);
        
        // Encrypt string
        char encrypted[BUFFER_SIZE];
        for (int i = 0; i < len; i++)
            encrypted[i] = encrypt_char(plain[i]);
            
        // Send encrypted text back to client
        write(client_fd, encrypted, len);
    }
}

/* Helper thread function that enables the server to quit */
void *quit(void *param) {
	while (1) {
		char buf[BUFFER_SIZE];
		gets(buf);
		if (strcmp(buf, ":q") == 0) {
			printf("Server quit at %s.\n", time_str());
			exit(0);
		}
		usleep(100000);
	}
}

int main(int argc, char *argv[]) {
    /* Network Setup */
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Error opening socket.\n");
        exit(1);
    }
    
    // Bind socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    int portno = 2050;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error binding.\n");
        exit(1);
    }
    listen(server_fd, 5);
    printf("Server initialized at %s.\n", time_str());

    /* Multi-threaded service */
    // Create quit thread
    pthread_t q_tid;
    pthread_attr_t q_attr;
    pthread_attr_init(&q_attr);
    pthread_create(&q_tid, &q_attr, quit, NULL);
    
    // Intialize semaphore
    sem_init(&service, 0, 2);
    
    // Create threads
    while (1) {
        // Wait for service to become available
        sem_wait(&service);
        
        // Accept client connection
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addr_len);
        if (client_fd < 0) {
            printf("Error accepting connection.\n");
            continue;
        }
        
        // Create server thread
        pthread_t s_tid; 
        pthread_attr_t s_attr;
        pthread_attr_init(&s_attr); 
        pthread_create(&s_tid, &s_attr, serve, &client_fd);
    }

    return 0;
}
