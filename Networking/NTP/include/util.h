#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>
#include <math.h>

#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "client.h"

int sendall(int s, unsigned char* buffer, size_t buf_size);
int receive_all(int s, unsigned char **buffer, size_t buf_size);
double collect_delay(uint64_t t1, uint64_t t4, uint64_t t2, uint64_t t3);
double collect_offset(uint64_t t1, uint64_t t4, uint64_t t2, uint64_t t3);
void delay();
double max_dis(double a, double b);
double min_dis(double a, double b);
void print_infos(char *host, int n, double dis_root, double dis, double delay, double offset);
#endif