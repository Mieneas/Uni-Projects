#ifndef BLOCK2_TEXT_FILE_H
#define BLOCK2_TEXT_FILE_H

#endif //BLOCK2_TEXT_FILE_H

//calculate the size of a file
long get_file_size(char *filename);

//splits file in lines
//return array[][];
int split_file(char *filename, char **array, int *sizes, int *index);

//return random number less max.
int get_random(int max);