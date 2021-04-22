#ifndef BLOCK5_NETWORK_H
#define BLOCK5_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

int connect_socket(struct addrinfo *res);

#endif //BLOCK5_NETWORK_H
