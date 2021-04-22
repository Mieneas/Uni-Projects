#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <time.h>
#include <netinet/in.h>

#define PKT_LEN  48

typedef struct _packet {
    uint64_t dis_root;
    uint64_t receive_timestamp_T2;
    uint64_t transmit_timestamp_T3;
} packet;


packet* packet_new();
uint8_t* packet_serialize();
packet* packet_decode(uint8_t* buffer, size_t buf_len);

#endif