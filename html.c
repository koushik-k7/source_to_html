#include "converter.h"
#include "parser.h"
#include "html.h"

/* To represent line numbers in browser */
extern int line;

/*
 * DESCRIPTION: This function store the token in the html file using span inline element 
 * INPUT: Pointer to Input_arg, token 
 * OUTPUT: void
 */
void update_html(Input_arg *src, Token_type token_value, char *token_str, int ch)
{
    FILE *fp = src -> fptr_dest_html_file;
    int flag = src -> number_flag;

    /* Check for token and store it in html file */

    if (token_value == numerical)
    {
	html_store_literal(fp, token_str, "numerical");
    }
    else if (token_value == str_literal)
    {
	html_store_literal(fp, token_str, "str-literal");
    }
    else if (token_value == char_literal)
    {
	html_store_literal(fp, token_str, "char-literal");
    }
    else if (token_value == macro)
    {
	html_store_literal(fp, token_str, "macro");
    }
    else if (token_value == keyword_data)
    {
	html_store_literal(fp, token_str, "keyword-data");
    }
    else if (token_value == keyword_nondata)
    {
	html_store_literal(fp, token_str, "keyword-nondata");
    } 
    else if (token_value == comment)
    {
	html_store_literal(fp, token_str, "comment");
    }
    else if (token_value == esc_literal)
    {
	html_store_literal(fp, token_str, "esc-literal");
    }
    else if (token_value == misc)
    {
	/* To store HTML entities <, >, & */
	if (ch == '<')
	{
	    fprintf(fp, "&lt;");
	}
	else if (ch == '>')
	{
	    fprintf(fp, "&gt;");
	}
	else if (ch == '&')
	{
	    fprintf(fp, "&amp;");
	}
	else if (ch == '\t')
	{
	    fprintf(fp, "        ");
	}
	else
	{
	    fprintf(fp, "%c", ch);
	}

	/* If line number enabled, display line number */
	if (ch == '\n' && flag)
	{
	    fprintf(fp, "<span class = \"numerical\">%3d. </span>", ++line);
	}
    }
    else 
    {
	fprintf(fp, "%s", token_str);
    }

}

/*
 * DESCRIPTION: This function store the token in the html file using span inline element 
 * INPUT: token and class
 * OUTPUT: void
 */
void html_store_literal(FILE *fp, char *str, char *class)
{
    fprintf(fp, "<span class = \"%s\">%s</span>", class, str);
}

/*
 * DESCRIPTION: This function initializes html file
 * INPUT: Pointer to Input_arg
 * OUTPUT: void
 */
void html_init(Input_arg *src)
{
    FILE *fp = src -> fptr_dest_html_file;

    fprintf(fp, 
	    "<!DOCTYPE html>\n"
	    "<html lang = \"en-US\">\n"
	    "<head>\n"
	    "<title> C to HTML </title>\n"
	    "<meta charset = \"UTF-8\">\n"
	    "<link rel=\"stylesheet\" href=\"styles.css\">\n"
	    "</head>\n"
	    );

    fprintf(fp,
	    "<body> \n"
	    "<pre>\n");

    if (src -> number_flag)
    {
	fprintf(fp, "<span class = \"numerical\">%3d. </span>", ++line);
    }

}

/*
 * DESCRIPTION: This function closes the html file
 * INPUT: Pointer to Input_arg
 * OUTPUT: void
 */
void html_conc(Input_arg *src)
{
    fprintf(src -> fptr_dest_html_file,
	    "</pre>\n"
	    "</body> \n"
	    "</html>\n"
	    );

}



