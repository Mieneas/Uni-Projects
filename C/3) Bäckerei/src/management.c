#include "./../include/management.h"
#include "./../include/order.h"
#include "./../include/supplier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct management{
    //TODO
    //ordes_in ist ein Ringpuffer, in dem die eingehenden Bestellungen vom Management platziert werden
    //ordes_in is a ringbuffer where the incomming orders are placed by the management

    pthread_mutex_t mut;
    pthread_cond_t cond;

    order_list* orders_in;

    //orders_finished is an array where finished orders are placed by the workers
    order* orders_finished[MAX_ORDERS];

    unsigned int num_worker_threads;
    FILE* order_file;
    forwarding_agent* fa;

};

management* management_create(char* filename, unsigned int num_worker_threads, forwarding_agent* fa){
    //TODO
    management* new_management = malloc(sizeof(management));
    if(new_management == NULL)
        return NULL;

    pthread_mutex_init(&new_management ->mut, NULL);
    pthread_cond_init(&new_management ->cond, NULL);

    new_management->orders_in = order_list_create();
    if(new_management->orders_in == NULL)
        return NULL;

    for(int i = 0; i<MAX_ORDERS;++i){
        new_management->orders_finished[i] = NULL;
    }

    new_management->num_worker_threads = num_worker_threads;

    new_management->order_file = fopen(filename,"r");
    if(new_management->order_file == NULL){
        printf("could not open %s\n",filename);
        return NULL;
    }

    new_management->fa = fa;


    return new_management;
}

//THREAD FUNCTION
//reads new orders from file and deposit orders in the orders_in ringbuffer
void* get_and_deposit_new_orders(void* man){
    management* m = (management*) man;
    char buffer[100];
    char* client;
    unsigned int muffin;

    int count_orders = 0;
    while(fgets(buffer,100,m->order_file) && count_orders <MAX_ORDERS){
        client = strtok(buffer,";\n");

        if(client == NULL){
            break;
        }

        muffin = atoi(strtok(NULL,";\n"));
        if(muffin != 0){
            order* new_order = order_create(client, muffin);
            ++count_orders;
            deposit_order(m->orders_in, new_order);
        }
    }
    // ADD NULL at the end of the buffer. NULL is the terminating signal for the workers
    for(unsigned int i = 0; i<m->num_worker_threads;++i){
        //++count;
        deposit_order(m->orders_in,NULL);
    }

    return NULL;
}

//FUNCTIONS CALLED BY WORKER THREADS
//worker threads deposit the order they finished in the orders_finished array
void deposit_finished_orders(management* m, order* o){
    //TODO

    pthread_mutex_lock(&m->mut);
    int i = 0;
    while (i < MAX_ORDERS) {
        if (m->orders_finished[i] == NULL){
            m->orders_finished[i] = o;
            pthread_mutex_unlock(&m->mut);
            return;
        }
        ++i;
        pthread_mutex_unlock(&m->mut);
        pthread_mutex_lock(&m->mut);
    }
    pthread_mutex_unlock(&m->mut);

    //to many orders have been executed
    order_destroy(o);
}

//returns the next order in the ringbuffer
order* get_order_from_management(management* m){
    order* o = get_order(m->orders_in);
    return o;
}

//FUNCTION CALLED BY BAKERY
unsigned int management_destroy(management* m){
    //TODO

    pthread_mutex_lock(&m ->mut);
    stop_forwarding_agent(m->fa);
    while(get_count(m->orders_in) > 0) {
    	pthread_mutex_unlock(&m ->mut);
        order *tmp = get_order(m->orders_in);

        order_destroy(tmp);

        pthread_mutex_lock(&m ->mut);
    }
    pthread_mutex_unlock(&m ->mut);
    order_list_destroy(m->orders_in);

    //Printing summaray
    pthread_mutex_lock(&m ->mut);
    unsigned int sum_of_muffin = 0;
    for(int i = 0; i<MAX_ORDERS && m->orders_finished[i] != NULL;++i){
    	pthread_mutex_unlock(&m ->mut);
        printf("client: %s \t muffin ordered: %u\n",get_client(m->orders_finished[i]), get_muffin(m->orders_finished[i]));

        sum_of_muffin += get_muffin(m->orders_finished[i]);
        order_destroy(m->orders_finished[i]);

        pthread_mutex_lock(&m ->mut);
    }
    pthread_mutex_unlock(&m ->mut);
    printf("sum of muffin: %u\n", sum_of_muffin);

    fclose(m->order_file);

    pthread_mutex_destroy(&m ->mut);
    pthread_cond_destroy(&m ->cond);
    free(m);

    return sum_of_muffin;
}


order_list* get_orders_in(management* m){
    return m->orders_in;
}



