#include "util.h"

int sendall(int s, uint8_t* buffer, size_t buf_size){
    size_t sent = 0;
    while(sent < buf_size){
       int n = send(s, buffer + sent, buf_size - sent, 0);
       if (n < 1){
           perror("send");
           return -1;
       }
       sent += n;
    }
    //
    return 0;
}

int receive_all(int s, uint8_t **buffer, size_t buf_size){
    *buffer = (uint8_t *)malloc(sizeof(uint8_t) * buf_size);
    size_t nbytes = 0;
    while(nbytes < buf_size){
        nbytes += recv(s, (*buffer) + nbytes, buf_size, 0);
        if (nbytes < 1){
            perror("send");
            return -1;
        }
    }
    return 0;
}

void delay(){
    sleep(8);
}

double collect_delay(uint64_t t1, uint64_t t2, uint64_t t3, uint64_t t4){
    int64_t a = (t4 - t1) - (t3 - t2);
    double b = a / (double) ((uint64_t) 1 << 32);
    return b/2;
}

double collect_offset(uint64_t t1, uint64_t t2, uint64_t t3, uint64_t t4){
    int64_t a = (t2 - t1) + (t3 - t4);
    double b = a / (double) ((uint64_t) 1 << 32);
    return b/2;
}

double max_dis(double a, double b){
    if(a > b)
        return a;
    return b;
}

double min_dis(double a, double b){
    if(a < b)
        return a;
    return b;
}

void print_infos(char *host, int n, double dis_root, double dis, double delay, double offset){
    printf("%s;%d;%f;%f;%f;%f\n", host, n, dis_root, dis, delay, offset);
}