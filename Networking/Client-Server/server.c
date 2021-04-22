//this server is written based on: http://www.tobscore.com/socket-programmierung-in-c/
//							  and: Beej's Guide to Network Programming.

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <memory.h>

#include "lib/server_functions.h"
#include "lib/server_loop.h"
#include "lib/text_file.h"


int get_row_col(char *filename, int *row, int *col){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "can not open file: %s\n", filename);
        return -1;
    }
    char c ='C';
    int current_col = 0;
    while (c != EOF){
        c = getc(file);
        current_col++;
        if(c == '\n') {
            if (*col < current_col)
                *col = current_col;
            *row = *row + 1;
        }
    }
    fclose(file);
    return 0;
}


int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "wrong input. Enter <int>:<port> <string>:<filename.txt>");
        return -1;
    }

    int array_size = 0; int col = -1;
    if(get_row_col(argv[2], &array_size, &col) == -1)
        exit(1);

    char *lines[array_size];
    memset(lines, 0, sizeof(lines));
    int lines_size[array_size];

    int signal2 = split_file(argv[2], lines, lines_size, &array_size);
    if(signal2 == -1)
        exit(1);
    if(array_size == 0){
        fprintf(stderr, "file contents just newline or one line without newline at end.");
        exit(1);
    }


    int socket_number = initial_Socket_bind(argv[1]);
    if(socket_number == -1){
        exit(1);
    }

    if(listen(socket_number, 7) == -1) {
        fprintf(stderr, "server fail at listen!\n");
        exit(1);
    }

    allow_external_signals();

    printf("server is waiting for connection...\n");

    accept_client(socket_number, lines, lines_size, array_size);

    return 0;
}