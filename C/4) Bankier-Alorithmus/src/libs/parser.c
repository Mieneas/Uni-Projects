#include "parser.h"

int m = 0;
int n = 0;

int tmp_array[100];
int num_of_A = 0;

state* create_state(int m, int n)
{
	state* s = (state*)malloc(sizeof(state));
	s->m = m;
	s->n = n;
	s->f = (int*)malloc(sizeof(int)*n);
	init_Q(&s->a_array);
	s->allocations = &s->a_array;

	return s;
}

void free_state(state* s)
{
	node* elem = s->allocations->head;
	while(elem!=NULL)
	{
		free((int*)elem->data);
		elem = elem->prev;
	}
	free_Q(s->allocations);

	free(s->f);
	free(s);
}

int fill_array(char* to_read, int* to_fill, int num)
{

	if(num<1)
	{
		printf("WARNING: detected array size less zero!\n");
		return 0;
	}

	char* ptr;
	ptr = strtok(to_read, ",;}");
	to_fill[0] = atoi((const char*)ptr);

	for(int i=1; i<num; i++)
	{
		ptr = strtok(NULL, ",;}");
		if(ptr == NULL)
		{
			printf("WARNING: an array parsed probably did not hold as many elements as expected!\n");
			return -1;
		} 
		to_fill[i] = atoi((const char*)ptr);
	}

	return 1;
}

state*read_data(char* name)
{
	FILE* file = fopen(name, "r");
	char line[300];

	if(file==NULL)
	{
		printf("ERROR: file not found\n");
		return NULL;
	}

	state* current_state = NULL;

	while(fgets(line, sizeof(line), file) != NULL)
	{
		if(m!=0 && n!=0 && (current_state == NULL))
		{
			current_state = create_state(m,n);
		} 

		switch(line[0])
		{
			case 'm':	m=atoi((const char*)(line+2));
						if(m<1 || m>9)
						{
							printf("ERROR: variable 'm' not in range [1 - 9]\n");
							fclose(file);
							return NULL;
						}
						break;

			case 'n':	n=atoi((const char*)(line+2));
						if(n<1 || n>9)
						{
							printf("ERROR: variable 'n' not in range [1 - 9]\n");
							fclose(file);
							return NULL;
						}
						break;

			case 'f':	if(fill_array(line+3, current_state->f, n)!=1)
						{
							printf("ERROR: detected a problem while parsing the f-array\n");
							return NULL;
						}
						break;
						
			case 'A':	if(fill_array(line+3, tmp_array, m*n)!=1)
						{
							printf("ERROR: detected a problem while parsing an A-array\n");
							return NULL;
						}

						node* new_alloc = create_node(NULL, m*n*sizeof(int));
						new_alloc->data = (void*) malloc(m*n*sizeof(int));
						memcpy(new_alloc->data, tmp_array, m*n*sizeof(int));
						
						append_node(new_alloc, current_state->allocations);
						break;
			default: break;
		}
	}
	
	fclose(file);
	return current_state;
}

void dequeue_allocation(queue* q, node* allocation)
{
	remove_node(allocation, q);
	free_matrix((matrix*)allocation->data);
	free_node(allocation);
}

void move_allocation_to_back(queue* q, node* allocation)
{
	remove_node(allocation, q);
	append_node(allocation, q);
}

void print_array(int* a, int len)
{
	if(len <1) return;

	printf("[%i", a[0]);
	for(int i=1; i< len; i++)
	{
		printf(" --> %i", a[i]);
	}
	printf("]\n");
}

void print_state(state* s)
{
	printf("m: %i   |   n: %i\n", s->m, s->n);
	printf("f:  ");
	print_array(s->f, s->m);
	printf("\n");

	if(s->allocations == NULL)
	{
		printf("ERROR: print_state()\n");
		return;
	}

	node* tmp = s->allocations->head;
	int num = 1;
	while(tmp!=NULL)
	{
		printf("A%02i:  ", num);
		print_array((int*)tmp->data, s->m * s->n);
		tmp = tmp->prev;
		num++;
	}
	printf("\n");
}