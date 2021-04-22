#include "packet.h"
#include <math.h>

packet* packet_new(){
   packet* p = (packet*) malloc(sizeof(packet));
    memset(p, 0, sizeof(packet));
   return p;
}


uint8_t* packet_serialize(){
   uint8_t* buffer = (uint8_t*) malloc(sizeof(uint8_t) * PKT_LEN);
   memset(buffer, 0, PKT_LEN);

   int version_mode = 35;
   *buffer = (buffer[4]) | version_mode;

   return buffer;
}


packet* packet_decode(uint8_t* buffer, size_t buf_len){
     if (buf_len < PKT_LEN){
        fprintf(stderr, "Buffer to short (%zu bytes) to decode packet!\n", buf_len);
        return NULL;
    }

    packet* p = packet_new();

    uint16_t sec = ntohs(*(uint16_t*)&buffer[8]);
    uint16_t nano = ntohs(*(uint16_t*)&buffer[10]);
    p ->dis_root = (uint64_t) sec << 32;
    p ->dis_root |= (uint64_t) nano << 16;

    uint32_t sec1 = (uint32_t)ntohl(*(uint32_t*)&buffer[32]);
    uint32_t nano1 = (uint32_t)ntohl(*(uint32_t*)&buffer[36]);
    p ->receive_timestamp_T2 = (uint64_t) sec1 << 32;
    p ->receive_timestamp_T2 |= nano1;


    nano1 = sec1 = 0;
    sec1 = (uint32_t)ntohl(*(uint32_t*)&buffer[40]);
    nano1 = (uint32_t)ntohl(*(uint32_t*)&buffer[44]);
    p ->transmit_timestamp_T3 = (uint64_t) sec1 << 32;
    p ->transmit_timestamp_T3 |= nano1;
    return p;
}