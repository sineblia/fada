/*
 * Copyright (c) 2023, Simone Bellavia <simone.bellavia@live.it>.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, 
        (char *)server->h_addr_list[0], 
        server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    while (1) {
        printf("client> ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
        
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        
        printf("Server: %s\n", buffer);

        if (strncmp("exit", buffer, 4) == 0) break;
    }

    close(sockfd);

    return 0;
}
