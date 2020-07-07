#include "SJN.h"
#include "../include/task.h"
#include "../include/Queue.h"
#include <stdlib.h>
#include <stdio.h>

static Queue *sjn_queue = NULL;//head
def_task *running_sjn = NULL;//finish_task_SJN, um task freizugeben.
int tackt_sjn = 0;//Tacktzähler für jeden Prozess
int g_tackt_sjn = 0;//Tacktzähler für jeden Prozess

int init_SJN()
{
    // TODO
    //alle Variable initialisieren.
    sjn_queue = malloc(sizeof(Queue));
    sjn_queue -> head = NULL;
    sjn_queue -> size = 0;
    sjn_queue -> comparator = NULL;

    //wurde alles richtig initialisiert?
    if(sjn_queue -> head != NULL || sjn_queue -> size != 0 || sjn_queue -> comparator != NULL)
        return 1;
    return 0;
}

void free_SJN()
{
    // TODO
    free(running_sjn);
    queue_free(sjn_queue);
}

void arrive_SJN(int id, int length)
{
    // TODO
    //prüfe, ob Eingabe korrekt
    if(id < 0 || length < 1){
        exit(1);
    }

    q_elem *temp = sjn_queue -> head;

    //Speicher für den neuen Prozess reservieren und alle Infos zuweisen.
    def_task *task = malloc(sizeof(def_task));
    task -> id = id;
    task -> length = length;

    //Ist die Queue leer?
    if(sjn_queue -> head == NULL){
        //falls ja, dann head zeigt auf den neuen Prozess.
        queue_push_to_front(sjn_queue, task);
    }
    else {
        //falls nicht, dann prüfe zuerst welcher Priorität hat der neue Prozess.
        q_elem *new_elem = malloc(sizeof(q_elem));
        new_elem ->task = task;
        if (sjn_queue->head->task->length <= new_elem->task->length) {

            while(temp->next != NULL && temp->next -> task->length <= length) {
                temp = temp->next;
            }
            //dann füge ihn in der richtigen Platz.
            new_elem->next = temp->next;
            temp->next = new_elem;
            sjn_queue->size++;
        } else {
            queue_push_to_front(sjn_queue, new_elem->task);
            free(new_elem);
        }
    }
}


def_task *tick_SJN() {
    // TODO
    tackt_sjn++;
    g_tackt_sjn++;
    //Dieser Variable bekommt den aktuellen Prozess aus der Queue. Das hilft mir bei der Entfernung des Prozess aus der Queue.

    finish_task_SJN();
    if(tackt_sjn == 1) {
        running_sjn = queue_poll(sjn_queue);
    }
    switch_task(running_sjn);

    //Wenn die Bedienzeit des Prozesses = die Tackte, dann heißt das, dass der Prozess ausgeführt wurde.
    if (running_sjn != NULL && tackt_sjn == running_sjn -> length) {
        tackt_sjn = 0;
        return running_sjn;
    }
    return NULL;
}

void finish_task_SJN()
{
    // TODO optional
    if(running_sjn != NULL && g_tackt_sjn == running_sjn ->length + 1){
        free(running_sjn);
        running_sjn = NULL;
        g_tackt_sjn =  1;
    }
}