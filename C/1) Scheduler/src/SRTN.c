#include "SRTN.h"
#include "../include/task.h"
#include "../include/Queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue *queue_srtn = NULL;
q_elem *current_elem = NULL;
int globaltakt = -1;


void destroy(q_elem *q){
    q_elem *temp = q->next;
    int  Lenght = 0;
    while(temp != NULL) //Solange die Liste nicht zu Ende ist, gebe alles frei.
    {
        Lenght++;
        temp = temp->next;
    }

    q_elem *current = q;
    for (int j =0; j<Lenght; j++){
        temp= current ->next;
        free(current->task);
        free(current);
        current = temp;
    }
}

int init_SRTN()
{
    // TODO
    //initalisieren
    queue_srtn = (Queue*) malloc(sizeof(Queue));
    queue_srtn -> comparator = NULL,
            queue_srtn -> size = 0;
    queue_srtn -> head = NULL;

    current_elem = queue_srtn->head;

    //current_elem->task=NULL;
    //switch_task(current_elem->task);

    //Felermeldung
    if(queue_srtn -> head != NULL || queue_srtn -> size != 0 || queue_srtn -> comparator != NULL){
        return  1;
    }

    return 0;
}

void free_SRTN()
{
    //destroy(current_elem);
    queue_free(queue_srtn);
}

void arrive_SRTN(int id, int length)
{
    //prüfe, ob Eingabe korrekt
    if(id < 0 || length < 1){
        exit(1);
    }

    //erstellt temporären task
    def_task *new = (def_task*) malloc(sizeof(def_task));
    new->length = length;
    new->id = id;

    //ordnet den task in die queue
    queue_push_to_front(queue_srtn, new);
}

def_task *tick_SRTN()
{
    // TODO
    globaltakt++; //zeigt bei welchem takt wir und befinden
    finish_task_SRTN();

    current_elem = queue_srtn->head;    //setzt das current elem am anfang
    q_elem *temp = queue_srtn->head;    //setzt temp an anfang

    int size = queue_size(queue_srtn);  //länge der schlange bestimmen für die Schleife

    //task ermitteln mit der kleinesten restzeit, und wird auf current elem gespeichert
    for(int i = 0; i < size; i++ ){
        //wenn das nächste elem die kleine restzeit hat so setzt es auf current elem
        if (temp->next != NULL && current_elem->task->length >= temp->next->task->length){
            current_elem = temp->next;
        }
        temp = temp->next;
    }



    if (current_elem == NULL) {
        return NULL;
    }
    switch_task(current_elem->task);
    //else:
    current_elem->task->length--; //bearbeitet die task
    return current_elem->task;
}

void finish_task_SRTN()
{

    //nimmt das element an erster stelle und entfernt es
    if (current_elem != NULL && current_elem->task->length == 0) {
        q_elem *tmp = queue_srtn->head;
        if (tmp == current_elem) {
            queue_srtn->head = current_elem->next;
        } else {
            for (int i = 0; i < queue_size(queue_srtn); i++) {
                if (tmp->next == current_elem) {
                    tmp->next = current_elem->next;
                    break;
                }
                tmp = tmp->next;
            }
        }
        running_task = NULL;
        free(current_elem->task);
        free(current_elem);
        queue_srtn->size--;
    }
}
