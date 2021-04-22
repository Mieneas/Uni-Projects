#include "network.h"


int connect_socket(struct addrinfo *res){
    int status;
    struct addrinfo* r;

    int sockfd = -1;
    for (r = res; r != NULL; r = r->ai_next){
        if((sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol)) == -1) {
            perror("peer: socket");
            continue;
        }
        status = connect(sockfd, r->ai_addr, r->ai_addrlen);
        if (status < 0){
            perror("connect failed");
            continue;
        }
        break;
    }
    if(r != NULL) {
        fprintf(stderr, "Connected to ntp_server\n");
        return sockfd;
    }
    return -1;
}