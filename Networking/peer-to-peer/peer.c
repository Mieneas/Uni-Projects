#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "packet.h"
#include "util.h"
#include "hash_table.h"
#include "neighbour.h"
#include "requests.h"


htable **ht = NULL;
rtable **rt = NULL;


peer *self = NULL;
peer *pred = NULL;
peer *succ = NULL;


int forward(peer *p, packet *pack) {
    if (peer_connect(p) != 0) {
        fprintf(stderr, "Failed to connect to peer %s:%d\n", p->hostname, p->port);
        return -1;
    }

    size_t data_len;
    unsigned char *raw = packet_serialize(pack, &data_len);
    int status = sendall(p->socket, raw, data_len);

    peer_disconnect(p);
    return status;
}


int proxy_request(server *srv, int csocket, packet *p, peer *n) {

    if (peer_connect(n) != 0) {
        fprintf(stderr, "Could not connect to peer %s:%d to proxy request for client!", n->hostname, n->port);
        return CB_REMOVE_CLIENT;
    }

    size_t data_len;
    unsigned char *raw = packet_serialize(p, &data_len);
    sendall(n->socket, raw, data_len);

    size_t rsp_len = 0;
    unsigned char *rsp = recvall(n->socket, &rsp_len);

    // Just pipe everything through unfiltered. Yolo!
    sendall(csocket, rsp, rsp_len);
    free(rsp);

    return CB_REMOVE_CLIENT;
}

int lookup_peer(uint16_t hash_id) {

    // We could see whether or not we need to repeat the lookup
    //

    packet *lkp = packet_new();
    lkp->flags = PKT_FLAG_CTRL | PKT_FLAG_LKUP;
    lkp->hash_id = hash_id;
    lkp->node_id = self->node_id;
    lkp->node_port = self->port;

    lkp->node_ip = peer_get_ip(self);

    forward(succ, lkp);
    return 0;
}

int handle_own_request(server *srv, client *c, packet *p) {
    packet *rsp = packet_new();

    if (p->flags & PKT_FLAG_GET) {
        htable *entry = htable_get(ht, p->key, p->key_len);
        if (entry != NULL) {
            rsp->flags = PKT_FLAG_GET | PKT_FLAG_ACK;

            rsp->key = (unsigned char *) malloc(entry->key_len);
            rsp->key_len = entry->key_len;
            memcpy(rsp->key, entry->key, entry->key_len);

            rsp->value = (unsigned char *) malloc(entry->value_len);
            rsp->value_len = entry->value_len;
            memcpy(rsp->value, entry->value, entry->value_len);
        } else {
            rsp->flags = PKT_FLAG_GET;
            rsp->key = (unsigned char *) malloc(p->key_len);
            rsp->key_len = p->key_len;
            memcpy(rsp->key, p->key, p->key_len);
        }
    } else if (p->flags & PKT_FLAG_SET) {
        rsp->flags = PKT_FLAG_SET | PKT_FLAG_ACK;
        htable_set(ht, p->key, p->key_len, p->value, p->value_len);

    } else if (p->flags & PKT_FLAG_DEL) {
        int status = htable_delete(ht, p->key, p->key_len);

        if (status == 0) {
            rsp->flags = PKT_FLAG_DEL | PKT_FLAG_ACK;
        } else {
            rsp->flags = PKT_FLAG_DEL;
        }


    } else {
        rsp->flags = p->flags | PKT_FLAG_ACK;
        rsp->key = (unsigned char *) strdup("Rick Astley");
        rsp->key_len = strlen((char *) rsp->key);
        rsp->value = (unsigned char *) strdup("Never Gonna Give You Up!\n");
        rsp->value_len = strlen((char *) rsp->value);
    }

    size_t data_len;
    unsigned char *raw = packet_serialize(rsp, &data_len);
    free(rsp);
    sendall(c->socket, raw, data_len);

    return CB_REMOVE_CLIENT;
}

int handle_packet_data(server *srv, client *c, packet *p) {
    uint16_t hash_id = pseudo_hash(p->key, p->key_len);
    fprintf(stderr, "Hash id: %d\n", hash_id);
    if (peer_is_responsible(pred->node_id, self->node_id, hash_id)) {
        fprintf(stderr, "We are responsible.\n");
        return handle_own_request(srv, c, p);
    } else if (peer_is_responsible(self->node_id, succ->node_id, hash_id)) {
        fprintf(stderr, "Successor's business.\n");
        return proxy_request(srv, c->socket, p, succ);
    } else {
        fprintf(stderr, "No idea! Just looking it up!.\n");
        add_request(rt, hash_id, c->socket, p);
        lookup_peer(hash_id); //TODO Add this to open lookup requests
        return CB_OK;
    }
}

int answer_lookup(packet *p, peer *n) {
    peer *questioner = peer_from_packet(p);

    if (peer_connect(questioner) != 0) {
        fprintf(stderr, "Could not connect to questioner of lookup at %s:%d\n!",
                questioner->hostname, questioner->port);
        peer_free(questioner);
        return CB_REMOVE_CLIENT;
    }

    packet *rsp = packet_new();
    rsp->flags = PKT_FLAG_CTRL | PKT_FLAG_RPLY;
    rsp->hash_id = p->hash_id;
    rsp->node_id = n->node_id;
    rsp->node_port = n->port;
    rsp->node_ip = peer_get_ip(n);


    size_t data_len;
    unsigned char *raw = packet_serialize(rsp, &data_len);
    free(rsp);
    sendall(questioner->socket, raw, data_len);
    peer_disconnect(questioner);
    peer_free(questioner);
    return CB_REMOVE_CLIENT;
}

int handle_packet_ctrl(server *srv, client *c, packet *p) {

    fprintf(stderr, "Handling control packet...\n");

    if (p->flags & PKT_FLAG_LKUP) {
        if (peer_is_responsible(pred->node_id, self->node_id, p->hash_id)) {
            // Our business
            fprintf(stderr, "Lol! This should not happen!\n");
            return answer_lookup(p, self);
        } else if (peer_is_responsible(self->node_id, succ->node_id, p->hash_id)) {
            return answer_lookup(p, succ);
        } else {
            // Great! Somebody else's job!
            forward(succ, p);
        }
    } else if (p->flags & PKT_FLAG_RPLY) {
        // Look for open requests and proxy them
        peer *n = peer_from_packet(p);
        for (request *r = get_requests(rt, p->hash_id); r != NULL; r = r->next) {
            proxy_request(srv, r->socket, r->packet, n);
            server_close_socket(srv, r->socket);
        }
        clear_requests(rt, p->hash_id);
    } else {

    }
    return CB_REMOVE_CLIENT;
}

int handle_packet(server *srv, client *c, packet *p) {
    if (p->flags & PKT_FLAG_CTRL) {
        return handle_packet_ctrl(srv, c, p);
    } else {
        return handle_packet_data(srv, c, p);
    }
}


int main(int argc, char **argv) {

    if (argc < 10) {
        fprintf(stderr, "Not enough args!\n");
    }

    uint16_t idSelf = strtoul(argv[1], NULL, 10);
    char *hostSelf = argv[2];
    char *portSelf = argv[3];

    uint16_t idPred = strtoul(argv[4], NULL, 10);
    char *hostPred = argv[5];
    char *portPred = argv[6];

    uint16_t idSucc = strtoul(argv[7], NULL, 10);
    char *hostSucc = argv[8];
    char *portSucc = argv[9];

    self = peer_init(idSelf, hostSelf, portSelf); //  Not really necessary but convenient to store us as a peer
    pred = peer_init(idPred, hostPred, portPred); //

    succ = peer_init(idSucc, hostSucc, portSucc);


    server *srv = server_setup(portSelf);
    if (srv == NULL) {
        fprintf(stderr, "Server setup failed!\n");
        return -1;
    }
    ht = (htable **) malloc(sizeof(htable *));
    rt = (rtable **) malloc(sizeof(rtable *));
    *ht = NULL;
    *rt = NULL;

    srv->packet_cb = handle_packet;
    server_run(srv);
    close(srv->socket);
}