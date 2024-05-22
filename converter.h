#ifndef CONVERTER_H
#define CONVERETR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Status to represent functions' return status */
typedef enum
{
    e_success,
    e_failure
} Status;

/* Struct which holds the FILE pointer to .c and .html files */
typedef struct input_arg
{
    char *src_c_file;
    char *dest_html_file;
    FILE *fptr_src_c_file;
    FILE *fptr_dest_html_file;
    int number_flag;
} Input_arg;





#endif
