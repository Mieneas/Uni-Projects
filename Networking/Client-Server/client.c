//this client is written based on: Beej's Guide to Network Programming.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

int main(int argc, char *argv[]){

    //arguments checker
    if (argc != 3){
        fprintf(stderr, "usage: <HOST_NAME> <PORT>\n");
        return 1;
    }
    struct addrinfo address_information, *add_information_port, *p;
    memset(&address_information, 0, sizeof(address_information));
    address_information.ai_family = AF_UNSPEC;
    address_information.ai_socktype = SOCK_STREAM;
    address_information.ai_flags = AI_PASSIVE;

    int status;
    if (status = getaddrinfo(argv[1], argv[2], &address_information, &add_information_port) != 0) {                                                         // MAYBE should be (status != 0) ??????????????
        fprintf(stderr, "getaddrinfo(..) routine failed\n");
        return -1;
    }

    int sockfd;
    for(p = add_information_port; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype,
                            p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            //printf("hereeeeeeeeee\n");
            //perror("client: connect");
            continue;
        }

        break;
    }

    if(p == NULL) {
        fprintf(stderr,"client: failed to connect\n");
        return 2;
    }

    //if there is no available server.
    if(!add_information_port){
        fprintf(stderr, "Failed to find a working host address\n");
        return -1;
    }

    freeaddrinfo(add_information_port);

    char *buffer = NULL;
    int rcvDataLength = 0;

    while (1) {
        buffer = calloc(50, sizeof(char));

        // perror if data is corrupted
        if ((rcvDataLength = recv(sockfd, buffer, 50, 0)) == -1) {
            perror("client: problem while receiving the data\n");
            free(buffer);
            return -19;
        }

        if (rcvDataLength == 0) {
            free(buffer);
            break;
        }
        fwrite(buffer, sizeof(char), rcvDataLength, stdout);
        free(buffer);
    }
    close(sockfd);


    return 0;
}