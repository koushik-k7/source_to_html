#ifndef PARSER_H
#define PARSER_H


#define BUFFER_SIZE 1024
#define TOTAL_KEYWORDS 34

/* enum which defines the token types */
typedef enum
{
    keyword_data,
    keyword_nondata,
    identifier,
    macro,
    numerical,
    char_literal,
    esc_literal,
    str_literal,
    comment,
    whitespace,
    tabspace,
    newline,
    misc,

} Token_type;

/* Function which parse the input file */
Status parse_file(Input_arg *src);

/* Function which stores respective tokens in char array */
Token_type store_identifier(Input_arg *src, int ch);
Token_type store_macro(Input_arg *src, int ch);
Token_type store_digits(Input_arg *src, int ch);
Token_type store_char(Input_arg *src, int ch);
Token_type store_string(Input_arg *src, int ch);
Token_type store_comment(Input_arg *src, int ch);
Token_type store_file(Input_arg *src, int ch);
Token_type store_esc_seq(Input_arg *src, int ch);
Token_type store_format_spec(Input_arg *src, int ch);

/* Function which identifies the token from .c file */
Token_type get_token(Input_arg *src, int ch);


#endif

