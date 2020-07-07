#include <string.h>
#include <stdio.h>
#include "../matrix.h"
#include "queue.h"

#ifndef PARSER_H
#define PARSER_H

//struct to hold all the data extracted from a file
typedef struct state {
	int m;
	int n;
	int* f;
	queue a_array;
	queue* allocations;
} state;


/*
	allocates and returns a new struct of type state
*/
state* create_state(int m, int n);


/*
	frees the given struct and its data
*/
void free_state(state* s);


/*
	converts a char array of numbers into the corresponding int array
*/
int fill_array(char* to_read, int* to_fill, int num);


/*
	reads the given file and returns its data as a struct of type state
*/
state* read_data(char* name);


/*
	dequeues the given node from the specified queue. Only to be used for removing "allocation"-nodes from their queues 
*/
void dequeue_allocation(queue* q, node* allocation);


/*
	removes the given element from the specified queue and appends it afterwards
*/
void move_allocation_to_back(queue* q, node* allocation);


/*
	prints the given array
*/
void print_array(int* a, int len);


/*
	prints the data of the given struct of type state
*/
void print_state(state* s);
#endif