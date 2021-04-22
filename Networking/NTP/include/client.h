#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "packet.h"
#include "network.h"
#include "util.h"

#define SERVER_PORT "123"


#define CB_REMOVE_CLIENT  (-1)
#define CB_OK 0

typedef struct _NTP_server{
    char *name;
    struct addrinfo hints;
    struct addrinfo* res;
} ntp_server;

typedef struct _client {
    ntp_server **servers;
    long Num_servers;
    long Num_requests;
    struct timespec origin_timestamp_sec_nano_T1;
    struct timespec client_receive_timestamp_sec_nano_T4;
    double dis_root;
    double dis;
    double delay;
    double offset;
    packet* receive_pack;
    packet* send_pack;
    uint8_t* send_buffer;
    uint8_t* receive_buffer;
} client;


ntp_server **init_all_ntp_servers(long servers_Num, char **all_ips);
void client_run(client *c);


#endif