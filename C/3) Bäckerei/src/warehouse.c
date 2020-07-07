#include "./../include/warehouse.h"// ./../include/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

 struct warehouse{
     unsigned int flour, sugar, choclate;
     int locked;

     unsigned int sugar_taken, flour_taken, choclate_taken;
     //TODO
     pthread_mutex_t mut;
     pthread_cond_t cond_flour1;
     pthread_cond_t cond_suger1;
     pthread_cond_t cond_cho1;
     pthread_cond_t cond_flour;
     pthread_cond_t cond_suger;
     pthread_cond_t cond_cho;
 };

 warehouse* warehouse_create(){
     warehouse* new_warehouse = (warehouse*) malloc(sizeof(warehouse));
     if(new_warehouse== NULL){
         printf("Unable to allocate memory in function %s()\n",__func__);
         return NULL;
     }
     new_warehouse->flour = 0;
     new_warehouse->sugar = 0;
     new_warehouse->choclate = 0;
     new_warehouse->locked = 0;

     //TODO
     pthread_mutex_init(&new_warehouse->mut, NULL);
     pthread_cond_init(&new_warehouse ->cond_flour, NULL);
     pthread_cond_init(&new_warehouse ->cond_flour1, NULL);
     pthread_cond_init(&new_warehouse ->cond_cho, NULL);
     pthread_cond_init(&new_warehouse ->cond_cho1, NULL);
     pthread_cond_init(&new_warehouse ->cond_suger, NULL);
     pthread_cond_init(&new_warehouse ->cond_suger1, NULL);

     new_warehouse->choclate_taken = 0;
     new_warehouse->flour_taken = 0;
     new_warehouse->sugar_taken = 0;
     return new_warehouse;
 }

//FUNCTIONS CALLED BY WORKER THREADS
//@param: amount of flour/sugar/choclate to be taken from warehouse wh

 unsigned int get_choclate(unsigned int amount, warehouse* wh){

     pthread_mutex_lock(&wh ->mut);
     while(wh ->choclate == 0) {
         pthread_cond_wait(&wh->cond_cho1, &wh->mut);
     }
     wh->choclate_taken += amount;
     wh->choclate -= amount;
     pthread_cond_signal(&wh ->cond_cho);
     pthread_mutex_unlock(&wh ->mut);

     return amount;
 }

 unsigned int get_flour(unsigned int amount, warehouse* wh){

     //TODO
     pthread_mutex_lock(&wh->mut);
     while(wh ->flour == 0) {
         pthread_cond_wait(&wh->cond_flour1, &wh->mut);
     }
     wh->flour_taken += amount;
     wh->flour -= amount;
     pthread_cond_signal(&wh ->cond_flour);
     pthread_mutex_unlock(&wh->mut);

     return amount;
 }

 unsigned int get_sugar(unsigned int amount, warehouse* wh){

     //TODO
     pthread_mutex_lock(&wh->mut);
     while(wh ->sugar == 0) {
         pthread_cond_wait(&wh->cond_suger1, &wh->mut);
     }
     wh->sugar_taken += amount;
     wh->sugar -= amount;
     pthread_cond_signal(&wh ->cond_suger);
     pthread_mutex_unlock(&wh->mut);

     return amount;
 }


//FUNCTIONS CALLED BY SUPPLIER THREADS
//@param: amount of flour/sugar/choclate to be store in warehouse wh
 void deposit_choclate(unsigned int amount, warehouse* wh){
     //TODO
     pthread_mutex_lock(&wh->mut);
     while(wh ->choclate == MAX_STORE_CAPACITY && wh ->locked != 1){
        pthread_cond_wait(&wh ->cond_cho, &wh ->mut);
     }
     if (wh->choclate + amount > MAX_STORE_CAPACITY) {
         wh->choclate = MAX_STORE_CAPACITY;
         pthread_cond_signal(&wh ->cond_cho1);
         pthread_mutex_unlock(&wh->mut);
     } else {
         wh->choclate += amount;
         pthread_cond_signal(&wh ->cond_cho1);
         pthread_mutex_unlock(&wh->mut);

     }
     return;
 }

 void deposit_sugar(unsigned int amount, warehouse* wh){
     //TODO

     pthread_mutex_lock(&wh->mut);
     while(wh ->sugar == MAX_STORE_CAPACITY && wh ->locked != 1){
        pthread_cond_wait(&wh ->cond_suger, &wh ->mut);
     }
     if (wh->sugar + amount > MAX_STORE_CAPACITY) {
         wh->sugar = MAX_STORE_CAPACITY;
         pthread_cond_signal(&wh ->cond_suger1);
         pthread_mutex_unlock(&wh->mut);
     } else {
         wh->sugar += amount;
         pthread_cond_signal(&wh ->cond_suger1);
         pthread_mutex_unlock(&wh->mut);
     }

     return;
 }

 void deposit_flour(unsigned int amount, warehouse* wh){
     //TODO
    pthread_mutex_lock(&wh->mut);
     while(wh ->flour == MAX_STORE_CAPACITY  && wh ->locked != 1){
        pthread_cond_wait(&wh ->cond_flour, &wh ->mut);
     }
     if (wh->flour + amount > MAX_STORE_CAPACITY) {
         wh->flour = MAX_STORE_CAPACITY;
         pthread_cond_signal(&wh ->cond_flour1);
         pthread_mutex_unlock(&wh->mut);
     } else {
         wh->flour += amount;
         pthread_cond_signal(&wh ->cond_flour1);
         pthread_mutex_unlock(&wh->mut);
     }

     return;
 }


//FUNCTIONS CALLED BY BAKERY
 void lock_warehouse(warehouse* wh){
     //TODO
     pthread_mutex_lock(&wh->mut);
     wh->locked = 1;
     pthread_cond_signal(&wh ->cond_suger);
     pthread_cond_signal(&wh ->cond_flour);
     pthread_cond_signal(&wh ->cond_cho);
     pthread_mutex_unlock(&wh->mut);
 }

 unsigned int get_used_resources(warehouse* wh, int type){
     if(type == SUGAR) return wh->sugar_taken;
     if(type == FLOUR) return wh->flour_taken;
     if(type == CHOCLATE) return wh->choclate_taken;
     return 0;
 }


 void warehouse_destroy(warehouse* wh){
     //TODO
     pthread_mutex_destroy(&wh ->mut);
     pthread_cond_destroy(&wh ->cond_flour);
     pthread_cond_destroy(&wh ->cond_flour1);
     pthread_cond_destroy(&wh ->cond_cho);
     pthread_cond_destroy(&wh ->cond_cho1);
     pthread_cond_destroy(&wh ->cond_suger);
     pthread_cond_destroy(&wh ->cond_suger1);
     free(wh);
     return;
 }
