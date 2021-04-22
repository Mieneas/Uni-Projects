#include <stdio.h>

#include "client.h"
#include "packet.h"

int main(int argc, char** argv) {

    if (argc < 3) {
        fprintf(stderr, "Not enough args!\n");
        exit(1);
    }

    client *me = (client *) malloc(sizeof(client));
    me ->servers = init_all_ntp_servers(argc-2, argv+2);
    me ->Num_servers = argc - 2;
    me ->Num_requests = atoi(argv[1]);

    client_run(me);
}
