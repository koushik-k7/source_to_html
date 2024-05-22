/*
 * AUTHOR : Koushik K
 * DESCRIPTION: This program converts the .c file to .html file to make it readable in the browser, with the added geature of syntax highlighting.
 * 		This feature highlights the keywords, string literal, char literal, numericals, and so by providing different colors to each of them.
 * INPUT : ./c_to_html_converter <file.c> [<file.html>] [-n]
 * OUTPUT : file.html file
 */

#include "converter.h"
#include "parser.h"

int line = 0;

Status validate_arguments(Input_arg **arg, int argc, char **argv);
Status open_files(Input_arg *arg);


int main(int argc, char **argv)
{
    Input_arg *arg;

    /* Validate input arguments */
    if (validate_arguments(&arg, argc, argv) == e_failure)
    {
	puts("Usage: ./c_to_html_converter <file1.c> [<file2.html>] [ -n ]");
	return 0;
    }
    puts("INFO: Argument validation successful");

    /* Open input and output files */
    if ( open_files(arg) == e_failure)
    {
	puts("Usage: ./c_to_html_converter <file1.c> [<file2.html>] [ -n ]");
	return 0;
    }
    puts("INFO: Files are opened successfully");

    /* File parsing */
    if (parse_file(arg) == e_success)
    {
	puts("INFO: HTML file created successfully");
    }
    else
    {
	puts("INFO: Failure in creating html");
    }
    

    return 0;

}

/*
 * DESCRIPTION: This function validates the command line arguments and return the status
 * INPUT: command line arguments
 * OUTPUT: success or failure
 */
Status validate_arguments(Input_arg **arg, int argc, char **argv)
{
    if (argc < 2)
    {
	puts("INFO: Invalid number of arguments");
	return e_failure;
    }

    *arg = malloc(sizeof(Input_arg));
    if (*arg == NULL)
    {
	puts("INFO: Failure in memory allocation");
	return e_failure;
    }

    /* Check for source C file */
    char *file;
    if ( (file = strrchr(argv[1], '.')) != NULL && strcmp(file, ".c") == 0 )
    {
	(*arg) -> src_c_file = argv[1];
    }
    else
    {
	puts("INFO: Invalid Source C file");
	return e_failure;
    }

    /* Check for Destination HTML file */
    if ( argv[2]  && (file = strchr(argv[2], '.')) != NULL && strcmp(file, ".html") == 0)
    {
	(*arg) -> dest_html_file = argv[2];
    }
    else
    {
	int len = strlen(argv[1]);
	char *str = malloc(len + 4);

	strcpy(str, argv[1]);

	strcpy(strrchr(str, '.'), ".html");
	str[len+4] = '\0';

	(*arg) -> dest_html_file = str;
    }

    /* Check for -n option */
    if (argv[argc-1] && strcmp(argv[argc-1], "-n") == 0)
    {
	(*arg) -> number_flag = 1;
    }
    else
    {
	(*arg) -> number_flag = 0;
    }

    return e_success;

}

/*
 * DESCRIPTION: This function opens the .c and .html files
 * INPUT: pointer to Input_arg
 * OUTPUT: success or failure
 */
Status open_files(Input_arg *arg)
{

    arg -> fptr_src_c_file = fopen(arg -> src_c_file, "r");
    if (arg -> fptr_src_c_file == NULL)
    {
	puts("INFO: Failure in opening file");
	return e_failure;
    }

    /* Check for empty source file */
    fseek(arg -> fptr_src_c_file, 0, SEEK_END);
    if (ftell(arg -> fptr_src_c_file) <= 1)
    {
	puts("INFO: Source file empty");
	return e_failure;
    }

    fseek(arg -> fptr_src_c_file, 0, SEEK_SET);

    arg -> fptr_dest_html_file = fopen(arg -> dest_html_file, "w");
    if (arg -> fptr_dest_html_file == NULL)
    {
	puts("INFO: Failure in opening file");
	return e_failure;
    }

    return e_success;

}



