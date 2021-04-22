#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include <unistd.h>

#include "text_file.h"

void accept_client(int server_socket, char **line, int *sizes, int array_size){
    struct sockaddr_storage client_addr;
    socklen_t size = sizeof(client_addr);
    while (1){
        int client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &size);
        printf("connected with %hu\n", client_addr.ss_family);
        if(client_socket == -1){
            fprintf(stderr, "accept client failed!\n");
            continue;
        }
        int random_line = get_random(array_size);

        //fwrite(line[random_line], sizeof(char), sizes[random_line], stdout);

        int s = sizes[random_line];
        if(!fork()){
            close(server_socket);
            //schleife
            /*
             if(send(client_socket, line[random_line], sizes[random_line], 0) == -1)
                fprintf(stderr, "send failed!\n");
            */

            while (s > 0) {
                s -= send(client_socket, line[random_line], sizes[random_line], 0);
            }
            if(s == -1)
                fprintf(stderr, "send failed!\n");

            close(client_socket);
            exit(0);
        }
        close(client_socket);
    }
}
