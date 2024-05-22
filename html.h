#ifndef HTML_H
#define HTML_H

/* Function which stores the token in html file */
void update_html(Input_arg *src, Token_type token_value, char *token_str, int ch);
void html_store_literal(FILE *fp, char *str, char *class);

/* Functions which initializes and closes the html file */
void html_init(Input_arg *src);
void html_conc(Input_arg *src);




#endif
