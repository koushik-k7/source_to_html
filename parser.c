#include "converter.h"
#include "parser.h"
#include "html.h"

#define KEYWORD_NONDATA_SIZE 14
#define KEYWORD_DATA_SIZE 20

/* globally declare a char pointer */
char *token_str;

/* keywords which are not data type */
const char *keywords_non_datatype[] = { "auto", "break", "case", "continue", "default", "do", "else", "for", "goto", "if", "return", "sizeof", "switch", "while" };

/* Keywords which are datatype */
const char *keywords_datatype[] = { "char", "const", "double", "enum", "extern", "float", "int", "long", "register", "short", "signed", 
    "static", "struct", "typedef", "union", "unsigned", "void", "volatile" };



/*
 * DESCRIPTION: This function parse the input .c file and stores the respective tokens in char array
 * INPUT: Pointer to Input_arg
 * OUTPUT: success or failure
 */
Status parse_file(Input_arg *src)
{

    FILE *src_fp = src -> fptr_src_c_file;


    int ch;

    token_str = malloc(BUFFER_SIZE);
    Token_type token_value;

    /* Html file initialization */
    html_init(src);

    while ((ch = getc(src_fp)) != EOF)
    {
	/* get appropriate token */
	token_value = get_token(src, ch);

	/* send token to output file construction */
	update_html(src, token_value, token_str, ch);
    }

    /* Html file finish */
    html_conc(src);

    return e_success;
}


/*
 * DESCRIPTION: This function parse the input .c file and return the token
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type get_token(Input_arg *src, int ch)
{
    /* Check for identifier */
    if ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' )
    {
	return store_identifier(src, ch);
    }
    /* Check for preprocessor directives */
    if ( ch == '#' )
    {
	return store_macro(src, ch);
    }
    /* Check for integer literal */
    if ( ch >= '0' && ch <= '9')
    {
	return store_digits(src, ch);
    }
    /* Check for escape sequence */
    if (ch == '\\')
    {
	return store_esc_seq(src, ch);
    }
    /* Check for char litearal */
    if ( ch == '\'') 
    {
	return store_char(src, ch);
    }
    /* Check for string literal */
    if ( ch == '\"')
    {
	return store_string(src, ch);
    }
    /* Check for comments */
    if (ch == '/')
    {
	return store_comment(src, ch);
    }
    else
    {
	return misc;
    }
}


/*
 * DESCRIPTION: This function parses the .c file and stores the identifier in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_identifier(Input_arg *src, int ch)
{
    FILE *fp = src -> fptr_src_c_file;
    int pos = 0;
    token_str[pos++] = ch;

    /* store identifier */
    ch = getc(fp);
    while ( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' )
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    token_str[pos] = '\0';
    fseek(fp, -1, SEEK_CUR);

    /* We can minimize this by using hash table */
    if (strlen(token_str) <= 8)
    {
	int i;
	/* Check for keywords */
	for (i = 0; i < KEYWORD_DATA_SIZE ; i++)
	{
	    if (strcmp(token_str, keywords_datatype[i]) == 0)
		return keyword_data;
	}

	for (i = 0; i < KEYWORD_NONDATA_SIZE; i++)
	{
	    if (strcmp(token_str, keywords_non_datatype[i]) == 0)
		return keyword_nondata;
	}
    }
    return identifier;
}


/*
 * DESCRIPTION: This function parses the .c file and stores the preprocessor directive in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_macro(Input_arg *src, int ch)
{
    FILE *dest_fp = src -> fptr_dest_html_file;
    FILE *fp = src -> fptr_src_c_file;

    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);

    while (ch != ' ' && ch != '\n' && ch != EOF)
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    token_str[pos] = '\0';

    /* Check for #include directive */
    if (strcmp(token_str, "#include") == 0)
    {
	update_html(src, macro, token_str, ch);
	pos = 0;

	while (ch != '\n' && ch != EOF)
	{
	    /* Store HTML entities for < and > characters */
	    if (ch == '<')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'l';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else if (ch == '>')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'g';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else
	    {
		token_str[pos++] = ch;
	    }
	    ch = getc(fp);
	}
	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);
	return str_literal;
    }
    else
    {
	while (ch != '\n' && ch != EOF)
	{
	    /* Check for string literal inside macro value*/
	    if (ch == '\"')
	    {
		token_str[pos] = '\0';
		update_html(src, macro, token_str, ch);
		store_string(src, ch);
		update_html(src, str_literal, token_str, ch);
		pos = 0;
	    }
	    /* Check for integer litearal */
	    else if (ch >= '0' && ch <= '9')
	    {
		token_str[pos] = '\0';
		update_html(src, macro, token_str, ch);
		Token_type t = store_digits(src, ch);
		if (t == numerical)
		    update_html(src, numerical, token_str, ch);
		else
		    update_html(src, macro, token_str,  ch);

		pos = 0;
	    }
	    /* Check for HTML entites <, > and & */
	    else if (ch == '<')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'l';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else if (ch == '>')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'g';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else if (ch == '&')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'a';
		token_str[pos++] = 'm';
		token_str[pos++] = 'p';
		token_str[pos++] = ';';
	    }
	    else
	    {
		token_str[pos++] = ch;
	    }

	    ch = getc(fp);
	}

    }


    token_str[pos] = '\0';
    fseek(fp, -1, SEEK_CUR);

    return macro;
}


/*
 * DESCRIPTION: This function parses the .c file and stores the char literal in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_char(Input_arg *src, int ch)
{
    FILE *fp = src -> fptr_src_c_file;
    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);

    int esc_flag = 0;

    /* Check for escape char literal */
    if (ch == '\\')
    {
	esc_flag = 1;
	token_str[pos++] = ch;
	ch = getc(fp);

	/* Hexa numerical esc */
	if ( ch == 'x')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	    for (int i = 1; i <= 2; i++)
	    {
		if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
		{
		    token_str[pos++] = ch;
		}
		else break;
		ch = getc(fp);
	    }
	}
	/* Check for octal escape seq*/
	else if ( ch >= '0' && ch <= '7')
	{
	    if (ch == '0')
	    {
		token_str[pos++] = ch;
		ch = getc(fp);
	    }

	    for (int i = 1; i <= 3; i++)
	    {
		if (ch >= '0' && ch <= '7')
		{
		    token_str[pos++] = ch;
		}
		else
		    break;

		ch = getc(fp);
	    }
	}
	else  
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}

    }
    else if (ch == '\'')
    {
	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);
	return identifier;
    }
    else 
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    /* Check for end single quote */
    if (ch == '\'')
    {
	token_str[pos++] = ch;
	token_str[pos] = '\0';
	if (esc_flag)
	{
	    return esc_literal;
	}
	return char_literal;
    }
    else
    {
	fseek(fp, -1, SEEK_CUR);
	return identifier;
    }

}


/*
 * DESCRIPTION: This function parses the .c file and stores the string literal in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_string(Input_arg *src, int ch)
{
    FILE *dest_fp = src -> fptr_dest_html_file;
    FILE *fp = src -> fptr_src_c_file;

    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);

    Token_type temp;

    /* check escape sequence and modifiers */
    while (ch != '\"' && ch != EOF)
    {
	/* Check for new line */
	if (ch == '\n')
	{
	    token_str[pos] = '\0';
	    update_html(src, str_literal, token_str, ch);
	    update_html(src, misc, token_str, '\n');
	    pos = 0;
	}
	/* Check for escape sequences */
	else if (ch == '\\')
	{
	    token_str[pos] = '\0';
	    update_html(src, str_literal, token_str,ch);
	    store_esc_seq(src, ch);
	    update_html(src, esc_literal, token_str, ch);
	    pos = 0;
	}
	/* Check for format specifiers */
	else if (ch == '%')
	{
	    /* Store format specifier */
	    token_str[pos] = '\0';
	    update_html(src, str_literal, token_str,ch);

	    temp = store_format_spec(src, ch);
	    update_html(src, temp, token_str, ch);
	    pos = 0;

	}
	/* Check for HTML entities */
	else if (ch == '&')
	{
	    token_str[pos++] = '&';
	    token_str[pos++] = 'a';
	    token_str[pos++] = 'm';
	    token_str[pos++] = 'p';
	    token_str[pos++] = ';';
	}
	else if (ch == '<')
	{
	    token_str[pos++] = '&';
	    token_str[pos++] = 'l';
	    token_str[pos++] = 't';
	    token_str[pos++] = ';';
	}
	else if (ch == '>')
	{
	    token_str[pos++] = '&';
	    token_str[pos++] = 'g';
	    token_str[pos++] = 't';
	    token_str[pos++] = ';';
	}
	else
	{
	    token_str[pos++] = ch;
	}

	ch = getc(fp);
    }

    if (ch == '\"')
    {
	token_str[pos++] = ch;
    }

    token_str[pos] = '\0';
    return str_literal;
}

/*
 * DESCRIPTION: This function parses the .c file and stores the comment in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_comment(Input_arg *src, int ch)
{

    FILE *dest_fp = src -> fptr_dest_html_file;
    FILE *fp = src -> fptr_src_c_file;
    
    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);
    /* Check for single line comment */
    if (ch == '/')
    {
	token_str[pos++] = ch;
	ch = getc(fp);

	while (ch != '\n' && ch != EOF)
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}

	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);

	return comment;
    }

    /* Check for multiline comments */
    if (ch == '*')
    {
	token_str[pos++] = ch;
	ch = getc(fp);

	while (ch != EOF)
	{
	    /* Check for new line and HTML entities */

	    if(ch == '\n')
	    {
		token_str[pos] = '\0';
		update_html(src, comment, token_str, ch);
		update_html(src, misc, token_str, '\n');
		pos = 0;
	    }
	    else if (ch == '<')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'l';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else if (ch == '>')
	    {
		token_str[pos++] = '&';
		token_str[pos++] = 'g';
		token_str[pos++] = 't';
		token_str[pos++] = ';';
	    }
	    else
	    {
		token_str[pos++] = ch;
	    }


	    if (ch == '*')
	    {
		ch = getc(fp);
		token_str[pos++] = ch;

		if (ch == '/')
		{
		    break;
		}
	    }
	    ch = getc(fp);
	}
	token_str[pos] = '\0';

	return comment;
    }
    else
    {
	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);
	return misc;
    }

}


/*
 * DESCRIPTION: This function parses the .c file and stores the integer literals in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_digits(Input_arg *src, int ch)
{
    FILE *fp = src -> fptr_src_c_file;
    int pos = 0;
    /* Integer */
    if (ch == '0')
    {
	token_str[pos++] = ch;
	/* Check for hexadecimal */
	ch = getc(fp);
	if (ch == 'X' || ch == 'x')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	    while ( (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') )
	    {
		token_str[pos++] = ch;
		ch = getc(fp);
	    }

	    token_str[pos] = '\0';
	    fseek(fp, -1, SEEK_CUR);
	    return numerical;
	}
	else if ( !((ch >= '0' && ch <= '9') || ch == '.'))
	{
	    fseek(fp, -1, SEEK_CUR);
	    token_str[pos] = '\0';
	    return numerical;
	}
    }

    token_str[pos++] = ch;
    ch = getc(fp);

    /* Check for float numericals and scientific notation */
    while (ch >= '0' && ch <= '9')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    if (ch == '.' || ch == 'e' || ch == 'E')
    {
	token_str[pos++] = ch;
	ch = getc(fp);

	while ((ch >= '0' && ch <= '9') || ch == 'e' || ch == 'E' || ch == '.')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}
	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);
	return numerical;
    }
    else
    {
	fseek(fp, -1, SEEK_CUR);
	token_str[pos] = '\0';
	return numerical;
    }

}

/*
 * DESCRIPTION: This function parses the .c file and stores the escape sequences in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_esc_seq(Input_arg *src, int ch)
{
    FILE *fp = src -> fptr_src_c_file;
    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);

    /* Hexa numerical literal */
    if ( ch == 'x')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
	for (int i = 1; i <= 2; i++)
	{
	    if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
	    {
		token_str[pos++] = ch;
	    }
	    else break;
	    ch = getc(fp);
	}
    }
    /* Octal numerical literal */
    else if ( ch >= '0' && ch <= '7')
    {
	if (ch == '0')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}

	for (int i = 1; i <= 3; i++)
	{
	    if (ch >= '0' && ch <= '7')
	    {
		token_str[pos++] = ch;
	    }
	    else
		break;

	    ch = getc(fp);
	}
    }
    else
    {
	token_str[pos++] = ch;
	token_str[pos] = '\0';
	return esc_literal;
    }

    token_str[pos] = '\0';
    fseek(fp, -1, SEEK_CUR);

    return esc_literal;
}


/*
 * DESCRIPTION: This function parses the .c file and stores the format specifier in char array 
 * INPUT: Pointer to Input_arg
 * OUTPUT: Token_type
 */
Token_type store_format_spec(Input_arg *src, int ch)
{
    FILE *fp = src -> fptr_src_c_file;
    int pos = 0;
    token_str[pos++] = ch;

    ch = getc(fp);


    /* Check for flags */
    while (ch == ' ' || ch == '+' || ch == '-' || ch == '#' || ch == '0')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    /* Check for minimum field width */
    while ((ch >= '0' && ch <= '9') || ch == '*' )
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }

    /* Check for precision */
    if (ch == '.')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
	while ((ch >= '0' && ch <= '9') || ch == '*')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}
    }

    /* Check for length modifier */
    if (ch == 'L' || ch == 'z')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
    }
    else if (ch == 'h')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
	if (ch == 'h')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}
    }
    else if (ch == 'l')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
	if (ch == 'l')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}
    }
    /* Check for selective scanf */
    else if (ch == '[')
    {
	token_str[pos++] = ch;
	ch = getc(fp);
	if (ch == ']')
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}

	while (ch != ']' && ch != '\n' && ch != EOF)
	{
	    token_str[pos++] = ch;
	    ch = getc(fp);
	}
    }


    /* Check for conversion specifier */
    if (ch == 'd' || ch == 'i' || ch == 'o' || ch == 'u' || ch == 'x' || ch == 'X')
    {
	token_str[pos++] = ch;
	token_str[pos] = '\0';
	return esc_literal;
    }
    else if (ch == 'f' || ch == 'F' || ch == 'e' || ch == 'E' || ch == 'g' || ch == 'G')
    {
	token_str[pos++] = ch;
	token_str[pos] = '\0';
	return esc_literal;
    }
    else if (ch == 'a' || ch == 'A' || ch == 'c' || ch == 's' || ch == 'p' || ch == 'n')
    {
	token_str[pos++] = ch;
	token_str[pos] = '\0';
	return esc_literal;
    }
    else
    {
	token_str[pos] = '\0';
	fseek(fp, -1, SEEK_CUR);
	return str_literal;
    }

}


