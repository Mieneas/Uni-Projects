#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/wait.h>
#include <sys/socket.h>

#include <netdb.h>
#include <unistd.h>
#include <signal.h>



int initial_Socket_bind(char *port){
    struct addrinfo address_information, *add_information_port;
    memset(&address_information, 0, sizeof(address_information));
    address_information.ai_family = AF_UNSPEC; //IP address is either IPv4 or IPv6.
    address_information.ai_socktype = SOCK_STREAM; //TCP.
    address_information.ai_flags = AI_PASSIVE; // use standard protocol for AF_UNSPEC


    memset(&add_information_port, 0, sizeof(add_information_port));
    int get_addr_sign = getaddrinfo(NULL, port, &address_information, &add_information_port);
    if(get_addr_sign != 0){
        fprintf(stderr, "failed at getaddinfo!\n");
        return -1;
    }

    int socket_number =  socket(add_information_port ->ai_family, add_information_port ->ai_socktype,
                                 add_information_port ->ai_protocol);
    if(socket_number == -1){
        fprintf(stderr, "socket initialization failed!\n");
        return socket_number;
    }

    int bind_ = bind(socket_number, add_information_port ->ai_addr, add_information_port ->ai_addrlen);
    if(bind_ == -1){
        close(socket_number);
        fprintf(stderr, "socket failed to bound with the port!");
        return bind_;
    }
    freeaddrinfo(add_information_port);
    return socket_number;
}

void signal_handler(int sig){
    int set_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = set_errno;
}

void allow_external_signals(){
    struct sigaction signal;
    signal.sa_handler = signal_handler;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = SA_RESTART;

    if(sigaction(SIGCHLD, &signal, NULL) == -1){
        fprintf(stderr, "sigaction failed!\n");
        exit(1);
    }
}