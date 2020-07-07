#include "RR.h"
#include "../include/task.h"
#include "Queue.h"
#include <stdlib.h>

static int zeitscheibe;// Zeitscheibe
int remaining_time; //verbleibende Zeit der Task
Queue* queue_rr;

int comparator(const void *p, const void *q)
{
    return 1;
}

int init_RR(int time_step)
{
    //Übergeben der Zeitscheibe mit time_step > 0
    if(time_step > 0){
        running_task = NULL;
        zeitscheibe = time_step;
        remaining_time = 0;

        queue_rr = queue_new(comparator);
        return 0;
    }

    else{
        return 1;
    }
}
void free_RR()
{
    queue_free(queue_rr);
}

void arrive_RR(int id, int length)
{
    //prüfe, ob Eingabe korrekt
    if(id < 0 || length < 1){
        exit(1);
    }

    //erstelle neue Task
    def_task* new = (def_task*) malloc(sizeof(def_task));
    if(!new){
        exit(1);
    }
    new->id = id;
    new->length = length;

    //packe die Task vorne in die Queue, da sie direkt als nächstes dran ist
    queue_offer(queue_rr, new);
}

def_task *tick_RR()
{

    //check, ob Task fertig
    if(running_task != NULL && running_task->length == 0){
        free(switch_task(queue_poll(queue_rr)));
        remaining_time = zeitscheibe;
    }
    //check ob prozess in warteschlange
    if(running_task == NULL && queue_size(queue_rr) > 0){
        switch_task(queue_poll(queue_rr));
        remaining_time = zeitscheibe;
    }
    //check ob Zeitscheibe abgelaufen,wenn tausche tasks
    if(remaining_time == 0 && queue_size(queue_rr) > 0){
        queue_push_to_back(queue_rr ,switch_task(queue_poll(queue_rr)));
        remaining_time = zeitscheibe;
    }
    //pro Tick eine Tasklänge weniger
    if(running_task != 0){
        running_task->length--;
    }
    if(remaining_time > 0){
        remaining_time--;
    }

    return NULL;

}

void finish_task_RR()
{
// TODO optional

}

