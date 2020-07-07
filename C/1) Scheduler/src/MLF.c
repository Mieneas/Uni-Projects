#include "MLF.h"
#include "../include/task.h"
#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

//Liste von Pointers, die auf die Köpfe der Queues zeigen.
typedef struct mlf_elem{
    int time;
    struct Queue *q;
    struct mlf_elem *next;
}mlf_elem;

//Head der Liste, die auf die Köpfe der Queues zeigen.
typedef struct Queue_list_head{
    int size;
    struct mlf_elem *head;
}MLF_list;

//Hier wird die Liste initialisiert, die auf die Köpfe der Queues zeigen.
MLF_list *MLF(MLF_list *q_list){
    q_list = malloc(sizeof(MLF_list));
    q_list ->head = NULL;
    q_list ->size = 0;//Anzahl der Queues.
    return q_list;
}

//Liste, die mir bei der Fortsetzung des Time_stap hilft.
typedef struct elem_fort{
    int remaining_time;
    int id;
    struct elem_fort *next;
}elem_fort;

//head der Liste, die mir bei der Fortsetzung des Time_stap hilft.
typedef struct head_fort{
    int size;
    struct elem_fort *head;
}head_fort;


//globale Valriablen:
MLF_list *mlf_list = NULL;//Liste, die die Pointers beinhaltet, die auf die Köpfe der Queues zeigen.
Queue *mlf_queue = NULL;//queue pointer.
//int preemitve_mlf = 0;//Verdrängungssignal
int tackt_mlf = 0;//takte für jeder Prozess.
int time_stap_mlf = 0;//Zeitscheibekontrolle
struct mlf_elem *h_q_pointer = NULL;//Pointer auf den pointer des Queuekopfes, der mir bei der Verdrängung hilft.
Queue *q_pointer = NULL;//zusätzlicher Queue Pointerder mir bei der Verdrängung hilft.
int q_ebene = 1;//welches Queue kommt danach.
head_fort *fort_list = NULL; //head der Datenstruktur head_fort

//diese Funktion berchnet die restliche Zeit für jeden Prozess.
void remaining(int q_time_stap, int id){
    elem_fort *elem = malloc(sizeof(elem_fort));
    elem ->remaining_time = q_time_stap - tackt_mlf + 1;
    elem ->id = id;
    elem ->next = fort_list ->head;
    fort_list ->head = elem;
    fort_list ->size++;
}

/*void preemptive(int pree) {
    //nur verdrängen, wenn im Zustand running überhaupt einen Prozess gibt.
    if(pree == 1 && running_task != NULL) {
        q_elem *preem = malloc(sizeof(q_elem));//Speicher für den versrängten Prozess allokieren und alle zugehörige Infos zuweisen.
        preem ->task = running_task;
        //Diesr Prozess wird an der zweite Stelle eingefügt, da er verdrängt wurde, und der Head wird im nächsten Schriit ausgeführt.
        preem ->next = q_pointer ->head ->next;
        q_pointer ->head ->next = preem;

        remaining(h_q_pointer ->time, running_task ->id);//Wievile Tackte können für diesen Prozess in dieser Queue zugeordnet werden.

        tackt_mlf = 1;
        time_stap_mlf = 1;
        preemitve_mlf = 0;//Schalte die Verdrängungssignal aus.
        q_pointer ->size++;
    }
    else if(pree == 1){
        preemitve_mlf = 0;
    }
}*/

int init_MLF(int time_step, int num_queues)
{
    // TODO
    mlf_list = MLF(mlf_list);//Pointers-Liste initialisieren.
    struct mlf_elem *new_elem = NULL;//Neuer Queue Zeiger
    struct mlf_elem *temp = NULL;
    int i = 0;

    //alle Queue erstellen.
    for(i = 0; i < num_queues; i++){
        //Queue initialisieren.
        mlf_queue = malloc(sizeof(Queue));
        mlf_queue ->head = NULL;
        mlf_queue ->size = 0;
        mlf_queue ->comparator = NULL;

        if(i == 0){//Erste Queue
            //erster Pointer auf die vorhin erstellte Queue zeigen lassen und alle zugehörige Infos zuweisen.
            mlf_list ->size++;
            mlf_list ->head = malloc(sizeof(struct mlf_elem));//Speicher für den Pointer allokieren.
            mlf_list ->head ->time = time_step;
            mlf_list ->head ->q = mlf_queue;
            mlf_list ->head ->next = NULL;

            h_q_pointer = mlf_list ->head;
            q_pointer = h_q_pointer ->q;
        } else{//andere Queues
            temp = mlf_list ->head;
            new_elem = malloc(sizeof(struct mlf_elem));//Speicher für den Pointer allokieren und initialisieren.
            new_elem ->q = mlf_queue;
            new_elem ->next = NULL;
            mlf_list ->size++;
            //diese Queue a Ende der Pointers Liste einfügen und alle zugehörige Infos zuweisen.
            while(temp ->next != NULL){
                temp = temp ->next;
            }
            if(i == num_queues - 2) {
                new_elem->time = temp->time * 2;
            }else{new_elem->time = 2147483647;}
            new_elem ->next = temp ->next;
            temp ->next = new_elem;
        }
    }

    //Hilfdatenstruktur, die mir bei der remaining_time hilft.
    fort_list = malloc(sizeof(head_fort));
    fort_list->head = NULL;
    fort_list ->size = 0;

    //kontrollieren, ob alles richtig initialisiert wurde.
    if(mlf_list ->head == NULL || mlf_list ->head ->q == NULL || mlf_list ->head ->q ->head != NULL)
        return 1;
    return 0;
}

void free_MLF()
{
    // TODO
    mlf_elem *temp = mlf_list ->head;
    mlf_elem *temp1 = mlf_list ->head ->next;

    //gebe die Hilfskiste frei:
    free(fort_list);

    //Erstmal alle Queues freigeben.
    while (temp1 != NULL) {
        queue_free(temp -> q);
        temp = temp1;
        temp1 = temp1 ->next;
    }

    //Dann die Liste der Pointers freigeben.
    while (mlf_list ->head != NULL){
        temp = mlf_list ->head;
        mlf_list ->head = mlf_list ->head ->next;
        free(temp);
    }
    free(mlf_queue);
    free(mlf_list);
    free(running_task);
    time_stap_mlf = 0;
    tackt_mlf = 0;
    q_ebene = 1;
}

void arrive_MLF(int id, int length)
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
    if(mlf_list -> head ->q ->head == NULL){
        //falls ja, dann head zeigt auf den neuen Prozess.
        queue_push_to_front(mlf_list ->head ->q, task);
    }
    else{
        //Neuer Prozess müssen am Ende der Queue stehen.
        q_elem *temp = mlf_list ->head ->q->head;
        if(temp != NULL) {
            while (temp->next != NULL) {
                temp = temp->next;
            }
            q_elem *new_elem_mlf = malloc(sizeof(q_elem));
            new_elem_mlf->task = task;
            new_elem_mlf->next = NULL;
            temp->next = new_elem_mlf;

            mlf_list ->head->q->size++;
        }
        //preemitve_mlf = 0;//Verdrängungssignal einschalten.
    }
}

def_task *tick_MLF() {
    // TODO
    tackt_mlf++;
    time_stap_mlf++;

    //Nach FIFO gibt es keine weitere Queue
    if(q_ebene < mlf_list ->size) {

        //Berechne last, das nutze ich um ein Prozess am Ende der näüchsten Queue zu verschieben.
        q_elem *last = h_q_pointer ->next ->q ->head;
        while (h_q_pointer ->next ->q ->head != NULL && last ->next != NULL){
            last = last ->next;
        }

        //Hier prüfe ich, ob der aktuellen Prozess in dieser Queue noch ausgeführt werden darf, falls Nein, dann wird das in der nächsten if Anweisung verschoben.
        if(running_task != NULL && fort_list ->head != NULL && running_task ->id == fort_list ->head ->id){
            fort_list ->head->remaining_time--;
            if(fort_list ->head ->remaining_time == 0){
                time_stap_mlf = h_q_pointer ->time + 1;

                //element für diesen Prozess in der Hilfsfunktion freigeben.
                elem_fort *temp = fort_list ->head;
                fort_list ->head = fort_list ->head ->next;
                free(temp);
            }
        }
        //Wenn der Prozess den aktuellen time_stap erreicht, dann muss er in der nächsten Q geschoben werden.
        if (time_stap_mlf == h_q_pointer->time + 1 && running_task->length != 0) {
            struct mlf_elem *temp = h_q_pointer;
            temp = temp->next;
            q_elem *temp1 = temp->q->head;

            if (temp1 == NULL) {
                queue_push_to_front(temp->q, running_task);
            } else {
                q_elem *verscieb = malloc(sizeof(q_elem));
                verscieb->task = running_task;
                verscieb->next = NULL;

                while (temp1->next != NULL) {
                    temp1 = temp1->next;
                }
                temp1->next = verscieb;
                temp->q->size++;
            }

            running_task = NULL;
            time_stap_mlf = 1;
            tackt_mlf = 1;
        }


        //preemptive(preemitve_mlf);//Soll Verdrängung passieren.
    }

    finish_task_MLF();//wurde den Prozess komplett ausgeführt.

    //Nach FIFO gibt es keine weitere Queue
    if(q_ebene < mlf_list ->size) {
        //Ist die aktuelle Q leer qeworden, dann muss der Prozessor zu der nächsten Q gehen.
        if (q_pointer->head == NULL && h_q_pointer->next->q->head != NULL && running_task == NULL) {
            h_q_pointer = h_q_pointer->next;
            q_pointer = h_q_pointer->q;
            q_ebene++;
        }
    }

    //Wenn die CUP nicht in der ersten Q und nicht in FIFO ist, dann muss der aktuellen Prozess verdrängt werden, falls Q_1 nicht leer ist.
    if(q_ebene > 1 && q_ebene != mlf_list ->size && mlf_list ->head->q->head != NULL){
        remaining(h_q_pointer ->time, running_task ->id);
        h_q_pointer = mlf_list ->head;
        q_pointer = h_q_pointer ->q;
        q_ebene = 1;
    }
        //In FIFO darf die CPU den aktuellen Prozess nicht verlassen.
    else if(q_ebene == mlf_list ->size && mlf_list ->head->q->head != NULL && running_task == NULL){
        h_q_pointer = mlf_list ->head;
        q_pointer = h_q_pointer ->q;
        q_ebene = 1;
    }


    if (tackt_mlf == 1) {//von Zustand ready zu running.
        running_task = queue_poll(q_pointer);
    }
    switch_task(running_task);


    if(running_task != NULL)//Nur minimieren, falls running_task nicht leer ist.
        running_task ->length--;

    //Wenn die Bedienzeit des Prozesses = die Tackte, dann heißt das, dass der Prozess ausgeführt wurde.
    if (running_task != NULL && 0 == running_task->length) {//nur zurückgeben, wenn der Prozess zu Ende ausgeführt wurde.
        tackt_mlf = 0;
        time_stap_mlf = 0;
        return running_task;
    }
    return NULL;
}

void finish_task_MLF()
{
    // TODO optional
    //den Task freigeben, wenn er ausgeführt wurde.
    if(running_task != NULL && 0 == running_task ->length){
        free(running_task);
        running_task = NULL;
    }
}