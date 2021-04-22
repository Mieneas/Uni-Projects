#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long get_file_size(char *filename){
    FILE *file_pointer = NULL;

    file_pointer = fopen(filename, "r");
    if(file_pointer == NULL){
        fprintf(stderr, "can not open file: %s", filename);
        return -1;
    }
    int sign = fseek(file_pointer, 0, SEEK_END);
    long size = ftell(file_pointer);
    fclose(file_pointer);

    if(size == 0){
        fprintf(stderr, "file is empty!");
        size = -1;
    }
    return size;
}

char *add_to_line(char *old_line, char sign, int index){
    char *new_line = (char*) malloc((index) * sizeof(char));
    memset(new_line, 0, index * sizeof(char));
    if(old_line != NULL)
        strncpy(new_line, old_line, (index - 1) * sizeof(char));
    new_line[index-1] = sign;
    if(old_line != NULL)
        free(old_line);
    return new_line;
}

int split_file(char *filename, char **line_array, int *sizes, int *index){
    FILE *file_pointer;
    long file_size = get_file_size(filename);
    if(file_size == -1){
        return (int)file_size;
    }

    file_pointer =fopen(filename, "r");
    if(file_pointer == NULL){
        fprintf(stderr, "can not open file: %s", filename);
        return -1;
    }

    char sign = 'C';
    char *line = NULL;
    int i = 1;

    int signal = 0;
    char *temp_line = NULL;
    int row = *index;
    *index = 0;
    int temp_line_size = 0;
    while (row != 0){
        fread(&sign, 1, 1, file_pointer);
        line = add_to_line(line, sign, i);
        file_size--; i++;
        if(sign == '\n' && strlen(line) != 1){
            if(signal != 1) {
                int line_size = (int) sizeof(char) * (i - 2);
                line_array[*index] = malloc(line_size);
                memcpy(line_array[*index], line, line_size);
                sizes[*index] = i - 2;
                *index = *index + 1;
                memset(line, 0, i-1);
                free(line);
                line = NULL;
                i = 1;
                row--;
            } else{
                signal = 0;
                int signal1 = 0;
                line_array[*index] = malloc((temp_line_size * sizeof(char)) + (strlen(line) * sizeof(char)));
                char *temp = malloc((temp_line_size * sizeof(char)) + (strlen(line) * sizeof(char)));
                sign = 'C';
                while (sign != '\n'){
                    if(signal < temp_line_size){
                        if(sign == '\000'){
                            temp[signal] = '\000';
                        }
                        else{
                            temp[signal] = temp_line[signal];
                        }
                        signal++;
                        sign = temp_line[signal];

                    } else {
                        if ((sign = line[signal1]) != '\n') {
                            temp[signal] = line[signal1];
                            signal++;
                            signal1++;
                        }
                    }
                }
                memcpy(line_array[*index], temp, signal);
                sizes[*index] = signal;
                *index = *index + 1;
                memset(line, 0, sizeof(line));
                free(line);
                free(temp_line);
                free(temp);
                line = NULL;
                temp_line = NULL;
                i = 1;
                signal = 0;
                row--;
            }
        }
        else if(sign == '\n' && strlen(line) == 1){
            if(temp_line != NULL){
                line_array[*index] = malloc((temp_line_size * sizeof(char)));
                memcpy(line_array[*index], temp_line, temp_line_size);
                sizes[*index] = temp_line_size;
                *index = *index + 1;
                memset(line, 0, sizeof(line));
                free(temp_line);
                temp_line = NULL;
            }
            free(line);
            line = NULL;
            i = 1;
            row--;
        }
        else if(sign == '\000'){
            signal = 0;
            int signal1 = 0;
            int byte_number = i-1;
            i = 0;
            //line_array[*index] = malloc((strlen(temp_line) * sizeof(char)) + (strlen(line) * sizeof(char)));
            char *temp = malloc((temp_line_size * sizeof(char)) + (byte_number * sizeof(char)));
            sign = 'C';
            while (i != temp_line_size + byte_number){
                if(signal < temp_line_size){
                    sign = temp_line[signal];
                    if(sign == '\000'){
                        temp[signal] = '\000';
                        signal++;
                        i++;
                    }else {
                        temp[signal] = temp_line[signal];
                        signal++;
                        i++;
                    }
                } else {
                    sign = line[signal1];
                    if(sign == '\000'){
                        temp[signal] = '\000';
                    }else {
                        temp[signal] = line[signal1];
                    }
                    signal++;
                    signal1++;
                    i++;
                }
            }
            temp_line_size += byte_number;
            i = 1;
            if(temp_line != NULL) {
                free(temp_line);
                temp_line = NULL;
            }
            temp_line = temp;
            signal = 1;
        }

    }
    if(line != NULL){
        sign = '\n';
        int line_size = (int)sizeof(char) * (i);
        line_array[*index] = malloc(line_size + sizeof(sign));
        strncpy(line_array[*index], line, line_size);
        strncat(line_array[*index], &sign, sizeof(sign));
        sizes[*index] = i-1;
        *index = *index + 1;
        memset(line, 0, sizeof(line));
        free(line);
    }
    fclose(file_pointer);
    return 0;
}

int get_random(int max){
    srand(time(0));

    return rand() % max;
}
