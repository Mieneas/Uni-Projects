#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

#ifndef BANKIER_H
#define BANKIER_H

/*
	checks whether with the given amount of resources any process could be executed at all

	matrix* f:	Available	(Vektor der insgesamt verfügbaren Ressourcen)
	matrix* G:	max 		(Gesamtanforderung)	

	returns: 	0 if a process requires more resources than available
				1 if all processes can be satisfied with the given resources
*/
int check_initial_state(matrix* f, matrix* G);


/*
	creates the virtual environment for the banker algorithm and calls it
	(the environment is needed because before an allocation is granted it needs to be checked in a secure simulation)

	matrix* B:	Allocation 	(Belegungsmatrix)
	matrix* R:	Need 		(Restanforderungsmatrix)
	matrix* f:	Available	(Vektor der aktuell freien Ressourcen)
	matrix* allocation:	matrix containing the amount of demanded resources
			(we guarantee that all  elements of this matrix are zero except the elemnts of one specific row
			 so that only exactly one process tries to allocate resources at the same time) 
	unsigned int ID: 	the ID (index) of the requesting process

	returns: 	1 if allocation was secure
				0 if allocation resulted in a deadlock/process being blocked
*/
int check_allocation(matrix* B, matrix* R, matrix* f, matrix* allocation, unsigned int ID);


/*
	performs the actual banker algorithm (checks the current state for being secure)

	matrix* B:	Allocation 	(Belegungsmatrix)
	matrix* R:	Need 		(Restanforderungsmatrix)
	matrix* f:	Available	(Vektor der aktuell freien Ressourcen)

	returns: 	1 if state was secure	(a valid order to finfish all processes was found)
				0 if state was not secure (no valid order to finfish all processes was found)
*/
int check_state(matrix* B, matrix* R, matrix* f);


/*
	prints a formatted combination of the matrices B (Allocation), R (Need) and f (Available)

	matrix* B:	Allocation 	(Belegungsmatrix)
	matrix* R:	Need 		(Restanforderungsmatrix)
	matrix* f:	Available	(Vektor der aktuell freien Ressourcen)

	returns: nuffin 
*/
void print_BRf(matrix* B, matrix* R, matrix* f);


#endif