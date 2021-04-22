#include "client.h"

void collect_all_t(uint64_t *t1, uint64_t *t4, client *my_client){
    uint32_t sec = my_client ->origin_timestamp_sec_nano_T1.tv_sec + 2208988800;
    uint32_t nano = (uint32_t)((double) my_client ->origin_timestamp_sec_nano_T1.tv_nsec / 1000000000 * ((uint64_t) 1 << 32));
    *t1 = (uint64_t) sec << 32;
    *t1 |= nano;

    sec = nano = 0;
    sec = my_client ->client_receive_timestamp_sec_nano_T4.tv_sec  + 2208988800;
    nano = (uint32_t)((double) my_client ->client_receive_timestamp_sec_nano_T4.tv_nsec / 1000000000 * ((uint64_t) 1 << 32));
    *t4 = (uint64_t) sec << 32;
    *t4 |= nano;

    my_client ->dis_root = my_client ->receive_pack ->dis_root / (double)((uint64_t) 1 << 32);
}

void reset_my_client(client *my_client){
    my_client ->origin_timestamp_sec_nano_T1.tv_sec = 0;
    my_client ->origin_timestamp_sec_nano_T1.tv_nsec = 0;
    my_client ->client_receive_timestamp_sec_nano_T4.tv_sec = 0;
    my_client ->client_receive_timestamp_sec_nano_T4.tv_nsec = 0;
    my_client ->dis_root = 0;
    my_client ->dis = 0;
    my_client ->delay = 0;
    my_client ->offset = 0;
    free(my_client ->receive_buffer);
    free(my_client ->receive_pack);
}

void client_run(client *my_client){
    int sock_disc;
    for(int i = 0; i < my_client ->Num_servers; i++){
        my_client ->send_pack = packet_new();
        my_client ->send_buffer = packet_serialize();

        double max_dispersion = 0;
        double min_dispersion = 0;

        for(int j = 0; j < my_client ->Num_requests; j++){
            sock_disc = connect_socket(my_client ->servers[i] ->res);
            sendall(sock_disc, my_client ->send_buffer, PKT_LEN);
            clock_gettime(CLOCK_REALTIME, &my_client ->origin_timestamp_sec_nano_T1);

            receive_all(sock_disc, &my_client ->receive_buffer, PKT_LEN);
            clock_gettime(CLOCK_REALTIME, &my_client ->client_receive_timestamp_sec_nano_T4);//maybe in receive_all.

            my_client ->receive_pack = packet_decode(my_client ->receive_buffer, PKT_LEN);

            uint64_t t1, t4;
            collect_all_t(&t1, &t4, my_client);

            my_client ->delay = collect_delay(t1, my_client ->receive_pack ->receive_timestamp_T2,
                                              my_client ->receive_pack ->transmit_timestamp_T3, t4);
            my_client ->offset = collect_offset(t1, my_client ->receive_pack ->receive_timestamp_T2,
                                                my_client ->receive_pack ->transmit_timestamp_T3, t4);

            if(max_dispersion == 0){
                max_dispersion = min_dispersion = my_client ->delay * 2;
            }
            max_dispersion = max_dis(max_dispersion, my_client ->delay * 2);
            min_dispersion = min_dis(min_dispersion, my_client ->delay * 2);

            my_client ->dis = max_dispersion - min_dispersion;

            print_infos(my_client ->servers[i] ->name, j, my_client ->dis_root, my_client ->dis, my_client ->delay, my_client ->offset);
            close(sock_disc);
            delay();
            reset_my_client(my_client);
        }
        freeaddrinfo(my_client ->servers[i] ->res);
        free(my_client ->send_pack);
        free(my_client ->send_buffer);
    }
}

//1.de.pool.ntp.org
ntp_server **init_all_ntp_servers(long servers_Num, char **all_ips) {

    ntp_server **servers_structures = (ntp_server **) malloc(sizeof(ntp_server *) * servers_Num);
    for(int i = 0; i < servers_Num; i++) {
        servers_structures[i] = (ntp_server *) malloc(sizeof(ntp_server));
        memset(servers_structures[i], 0, sizeof(ntp_server));

        memset(&servers_structures[i]->hints, 0, sizeof(struct addrinfo));
        servers_structures[i]->hints.ai_family = AF_INET; //AF_UNSPEC;
        servers_structures[i]->hints.ai_flags = AI_PASSIVE;
        servers_structures[i]->hints.ai_socktype = SOCK_DGRAM; // SOCK+STREAM;

        int status = getaddrinfo(all_ips[i], SERVER_PORT, &servers_structures[i]->hints, &servers_structures[i]->res);
        if (status != 0) {
            perror("getaddrinfo");
        }
        servers_structures[i] ->name = all_ips[i];
    }
    return servers_structures;
}