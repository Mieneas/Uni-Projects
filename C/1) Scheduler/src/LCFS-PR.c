#include "LCFS-PR.h"
#include "../include/task.h"
#include "../include/Queue.h"
#include <stdlib.h>
#include <stdio.h>

Queue *lcfs_queue = NULL;//die Queue
int tackt_lcfs = 0;//Tacke von Jedem Prozess
int g_tackt_lcfs = 0;//gesamte Tackte
def_task * running_lcfs = NULL;//running Zustand
int preemitve_lcfs = 0;//Um festzustellen, ob eine Verdrängung passieren muss.

int init_LCFS_PR()
{
    // TODO
    //alle Variable initialisieren.
    lcfs_queue = malloc(sizeof(Queue));
    lcfs_queue -> head = NULL;
    lcfs_queue -> size = 0;
    lcfs_queue -> comparator = NULL;

    //wurde alles richtig initialisiert?
    if(lcfs_queue -> head != NULL || lcfs_queue -> size != 0 || lcfs_queue -> comparator != NULL)
        return 1;
    return 0;
}

void free_LCFS_PR()
{
    // TODO
    free(running_lcfs);
    queue_free(lcfs_queue);
}

void arrive_LCFS_PR(int id, int length)
{
    // TODO
    //prüfe, ob Eingabe korrekt
    if(id < 0 || length < 1){
        exit(1);
    }

    //Speicher für den neuen Prozess reservieren und alle Infos zuweisen.
    def_task *task = malloc(sizeof(def_task));
    task -> id = id;
    task -> length = length;

    //Ist die Queue leer?
    if(lcfs_queue -> head == NULL && running_lcfs == NULL){
        //falls ja, dann head zeigt auf den neuen Prozess.
        queue_push_to_front(lcfs_queue, task);
    }
    else if(running_lcfs != 0){
        //Neuer Prozess verdrängt direkt den aktuellen Prozess, der im Zustand reunning ist.
        queue_push_to_front(lcfs_queue, task);//erst der neue Prozess im Queue am Head einfügen.
        preemitve_lcfs = 1;//Verdrängungssignal einschalten.
    }
}

def_task *tick_LCFS_PR()
{
    // TODO
    tackt_lcfs++;
    g_tackt_lcfs++;

    if(preemitve_lcfs == 1){ //Ist die Verdrängungssignal eingeschaltet, dann verdränge den laufenden Prozess.
        q_elem *temp = lcfs_queue -> head;
        q_elem *preem = malloc(sizeof(q_elem));
        preem ->task = running_lcfs;
        preem ->next = NULL;
        while (temp ->next != NULL){
            temp = temp ->next;
        }
        preem ->next = temp ->next;
        temp ->next = preem;

        tackt_lcfs = 1;
        preemitve_lcfs = 0;//Schalte die Verdrängungssignal aus.
        lcfs_queue ->size++;
    }

    finish_task_LCFS_PR();//wurde den Prozess komplett ausgeführt.
    if(tackt_lcfs == 1) {
        running_lcfs = queue_poll(lcfs_queue);//von Zustand ready zu running.
    }
    switch_task(running_lcfs);
    if(running_lcfs != NULL)
        running_lcfs ->length--;

    //Wenn die Bedienzeit des Prozesses = die Tackte, dann heißt das, dass der Prozess ausgeführt wurde.
    if (running_lcfs != NULL && 0 == running_lcfs -> length) {//nur zurückgeben, wenn der Prozess zu Ende ausgeführt wurde.
        tackt_lcfs = 0;
        return running_lcfs;
    }
    return NULL;
}

void finish_task_LCFS_PR()
{
    // todo optional
    //den Task freigeben, wenn er ausgeführt wurde.
    if(running_lcfs != NULL && 0 == running_lcfs ->length){
        free(running_lcfs);
        running_lcfs = NULL;
    }
}