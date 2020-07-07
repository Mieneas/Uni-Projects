#include "matrix.h"

matrix* create_matrix(unsigned int m, unsigned int n, int* numbers)
{
	/*TODO:
			- Argumente auf Gueltigkeit ueberpruefen
			- neue Matrix erstellen und zurückgeben (Pointer!!!)
			- (das Array soll dabei zwar kopiert, aber ansonsten UNVERAENDERT abgespeichert werden.
			  Dies ist für die automatischen Tests unerlässlich)

	*/
	if (m < 1 || n < 1){
	    printf("Invalid arguments at create_matrix(..):\nMatrix dimensions:\nheight <unsigned int> and width <unsigned int>.\n");
	    return NULL;
	}

	if (!numbers) {
	    printf("Invalid argument at create_matrix(..):\nMatrix isn't allocated.\n");
	    return NULL;
	}

	matrix *mat = (matrix *)malloc(sizeof(matrix));
	mat ->m = m;
	mat ->n = n;
    mat ->elements = calloc(m * n, sizeof(int));

	unsigned int i, j, counter = 0;
	for(i = 0; i < m; i++){
	    for(j = 0; j < n; j++){
	        mat ->elements[i*n + j] = numbers[counter++];
	    }
	}
	return mat;
}

matrix* create_matrix_from_row(unsigned int m, unsigned int row_nr, matrix* row)
{
	/*TODO
			-Argumente auf Gueltigkeit ueberpruefen
			-Erstellen einer neuen Matrix (Initialisierung mit 0)
			-Kopieren der gegebenen Zeile an die richtige Stelle
	*/
	if(m < 1 || row_nr > m - 1 || row_nr < 0 || row == NULL){
	    printf("Invalid arguments at create_matrix_from_row(..): \n"
               "1) height (@param m) and row index (@ param row_nr) must be non-zero <unsigned int>\n"
               "2) row index (@param row_nr) is out of boundary, or\n"
               "3) the passed matrix is NULL.\n");
	    return NULL;
	}

	matrix *new_matrix = malloc(sizeof(matrix));
	new_matrix ->n = row->n;
	new_matrix ->m = m;
	new_matrix ->elements = calloc(m * row->n, sizeof(int));

    unsigned int i;
    for(i = 0; i < m; i++){
        new_matrix ->elements[new_matrix->n * row_nr + i] = row ->elements[i];
    }

	return new_matrix;
}

void free_matrix(matrix* matrix)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Matrix und alle dynamisch reservierten elemente der Matrix freigeben
	*/
	if (matrix) {
        free(matrix->elements);
        free(matrix);
	}

}

matrix* duplicate_matrix(matrix* old)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Erstellen und Zurückgeben einer NEUEN Matrix mit genau den selben Parametern der gegebenen Matrix
	*/

	if(!old){
	    printf("Invalid argument at duplicate_matrix(..):\nNull argument passed.\n");
	    return NULL;
	}

	matrix * new = malloc(sizeof(matrix));
	new ->n = old ->n;
	new ->m = old ->m;
	new ->elements = calloc(old->n*old->m, sizeof(int));

	unsigned int i;
	for(i = 0; i < new->m*new->n; i++){
	    new ->elements[i] = old ->elements[i];
	}

	return new;
}

matrix* add_matrix(matrix* a, matrix* b)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Elementweises Addieren von a und b (a+b)
			-Rueckgabe des Ergebnisses
	*/
	if(a == NULL || b == NULL){
        if (!(a || b)) {
            printf("Invalid Arguments at add_matrix(..):\nBoth passed matrices are NULL.\n");
            return NULL;
        }

        if (!a) return b;
        else return a;

	}

	if (a->m != b->m || a->n != b->n) {
	  printf("Invalid matrix addition operation: both matrices must have same dimestions.");
	  return NULL;
	}


	unsigned int i;
	for(i = 0; i < a->n * a->m; i++){
	    a ->elements[i] = a ->elements[i] + b ->elements[i];
	}


	return a;
}

matrix* subtract_matrix(matrix* a, matrix* b)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Elementweises Subtrahieren von a und b (a-b)
			-Rueckgabe des Ergebnisses
	*/

    if(a == NULL || b == NULL){
        if (!(a || b)) {
            printf("Invalid Arguments at subtract_matrix(..):\nBoth passed matrices are NULL.\n");
            return NULL;
        }

        if (!a) return b;
        else return a;

    }

    if (a->m != b->m || a->n != b->n) {
        printf("Invalid matrix addition operation: both matrices must have same dimestions.");
	return NULL;
    }

    int i;
    for(i = 0; i < a->n * a->m; i++){
        a ->elements[i] = a ->elements[i] - b ->elements[i];
    }

    /*
    free(b ->elements);
    free(b);*/

    return a;
}

int get_elem_of_matrix(matrix* matrix, unsigned int i, unsigned int j)
{
	/*TODO
		-Argumente auf Gueltigkeit ueberpruefen
		-Rueckgabe des Elements an den Indizes i und j	
	*/
    if(matrix == NULL){
        printf("Argument error at get_elem_of_matrix(..): passed matrix is NULL.\n");
        return -2;
    }

	if(i < 0 || i > matrix ->m - 1 || j < 0 || j > matrix ->n - 1){
        printf("Argument error at get_elem_of_matrix(..): Out of boundary matrix dimensions.\n");
        return -1;
	}

	return matrix ->elements[ i * matrix ->n + j];
}

int check_matrix(matrix* matrix)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Pruefen, ob die Elemente der Matrix <, > oder = 0 sind
			->sobald ein Element <0 ist, soll -1 zurückgegeben werden
			->wenn alle Elemente 0 sind, soll 0 zurückgegeben werden
			->ansonsten soll 1 zurückgegeben werden
	*/

	if(matrix == NULL){
	    printf("Argument error at check_matrix(..): passed matrix is NULL.\n");
	    return 0;
	}

	unsigned int i = 0;
	for(i = 0; i < matrix->m* matrix->n; i++){
	    if(matrix ->elements[i] < 0)
            return -1;
	    else if(matrix ->elements[i] != 0){
	        return 1;
	    }
	}

	return 0;
}

matrix* get_row_from_matrix(matrix* mat, unsigned int m)
{
	/*TODO:
			-Argumente auf Gueltigkeit ueberpruefen
			-Rueckgabe der m-ten Zeile der Matrix (Die Zeile selbst ist auch wieder eine Matrix mit genau einer Zeile)
	*/
	if(mat == NULL || m < 0 || m > mat ->m - 1){
	    printf("Argument error at get_row_from_matrix(..):\n"
               "1) passed matrix is NULL, or\n"
               "2) height is not in [0, height - 1].\n");
	    return NULL;
	}
	matrix * row = (matrix *)malloc(sizeof(matrix));
	row ->m = 1;
	row ->n = mat ->n;
	row->elements = calloc(mat ->n, sizeof(int));

	unsigned int i;
	for(i = 0; i < mat ->n; i++){
	    row ->elements[i] = mat ->elements[mat ->n * m + i];
	}

	return row;
}

void clear_row_of_matrix(matrix* matrix, unsigned int m)
{
	/*TODO
			-Argumente auf Gueltigkeit ueberpruefen
			-Die m-te Zeile der gegebenen Matrix nullen (alle Einträge auf '0' setzen)
	*/
    if(matrix == NULL || m < 0 || m > matrix ->m - 1){
        printf("Argument error at clear_row_of_matrix(..):\n"
               "1) passed matrix is NULL, or\n"
               "2) height is not in [0, height - 1].\n");
        return;
    }

    unsigned int i;
    for(i = 0; i < matrix ->n; i++){
        matrix ->elements[matrix ->n * m + i] = 0;
    }

}

void print_matrix(matrix* matrix)
{
	if(matrix == NULL)
	{
		printf("ERROR: print_matrix() | given matrix is NULL.\n");
		return;
	}


	//determines the largest element
	int max = get_elem_of_matrix(matrix, 0,0);
	for(int i = 0; i< matrix->m; i++)
	{
		for(int j = 0; j < matrix->n; j++)
		{
			int current = get_elem_of_matrix(matrix, i,j);
			max = (current>max) ? current : max;
		}
	}

	int grade = 1;	//represents the number of digits of the largest element
	while(max > 9)
	{
		max/=10;
		grade++;
	}

	//prints upper line
	printf(" ");
	for(int i = 0; i< ((grade + 1)*matrix->n +1); i++)
	{
		printf("-");
	}
	printf("\n");

	//prints body
	for(int i = 0; i< matrix->m; i++)
	{
		printf("| ");
		for(int j = 0; j < matrix->n; j++)
		{
			printf("%*i ", grade, get_elem_of_matrix(matrix, i, j));
		}
		printf("|\n");
	}

	//prints lower line
	printf(" ");
	for(int i = 0; i< ((grade + 1)*matrix->n +1); i++)
	{
		printf("-");
	}
	printf("\n");
}


//===============================================================================

/*TODO:
	Hier koennen bei Bedarf eigene Funktionen implementiert werden
*/


//===============================================================================
