/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

// function to handle errors with custom messages
void error(const char *msg){
    perror(msg);
    exit(1);
}

// function to initialize and return a server socket
int init_server(int port){
    int sockfd;
    struct sockaddr_in serv_addr;

    // creates a socket. AF_INET indicates the use of IPv4, SOCK_STREAM of TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("ERROR opening socket");

    // resets the serv_addr structure, to avoid residual data
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // sets the server parameters
    // AF_INET for IPv4, INADDR_ANY to listen on all server IP addresses
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // IP address (anyone)
    serv_addr.sin_port = htons(port); // port number, converted in network byte order

    // associates the socket to the IP address and port specified
    if(bind(sockfd, (struct sockaddr * ) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    return sockfd; // returns the descriptor of the new configured socket
}

// function to start listening for clients
void start_listening(int sockfd){
    listen(sockfd, 5);
}

// function to accept connection from a client
int accept_client(int sockfd, struct sockaddr_in *cli_addr){
    socklen_t clilen = sizeof(*cli_addr); // determines the client address length

    // accepts a client connection. stops the execution until a connection arrives
    int newsockfd = accept(sockfd, (struct sockaddr *) cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    return newsockfd; // returns the descriptor of the accepted configured connection's socket
}

// main function
int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno;
    struct sockaddr_in cli_addr;
    char buffer[256];
    int n;

    if (argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    sockfd = init_server(portno);

    listen(sockfd, 5);

    // loop to accept multiple connections
    while(1) {
        // accepts a connection from a client
        newsockfd = accept_client(sockfd, &cli_addr);

        // resets the buffer and reads data from client
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) error("ERROR reading from socket");

        // prints the message received from the client
        printf("Here's the message: %s\n", buffer);

        // close the socket of a specific connection, but the server remains in listening for other connections
        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
