#ifndef RNVS_REFERENCE_REQUESTS_H
#define RNVS_REFERENCE_REQUESTS_H

#include "uthash.h"
#include "server.h"
#include "packet.h"

typedef struct _request {
    packet *packet;
    int socket;
    struct _request *next;
} request;

typedef struct _rtable {
    uint16_t hash_id;
    request *open_requests;
    UT_hash_handle hh;
} rtable;

void add_request(rtable **table, uint16_t hash_id, int socket, const packet *packet);

request *get_requests(rtable **table, uint16_t hash_id);

void clear_requests(rtable **table, uint16_t hash_id);

#endif //RNVS_REFERENCE_REQUESTS_H
