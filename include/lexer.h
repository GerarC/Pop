#ifndef LEXER_H
#define LEXER_H

#define MAX_NAME_SIZE 128
#define MAX_TOKEN_STRING_SIZE 256

/* Enum with all the kinds of tokens
 * */
typedef enum _token_type_e {
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_DIV,
	TOKEN_MULT,
	TOKEN_NUMBER,
	TOKEN_VARIABLE,
	TOKEN_INVALID
} TokenType;

/* Structure that saves the exact location of a token in the file
 * */
typedef struct _token_location_t {
	int line, col;
	char file[MAX_NAME_SIZE];
} TokenLocation;

/* Token structure represents a piece of code in its most important information
 * */
typedef struct _token_t {
	TokenType type;
	TokenLocation location;
	int length;
	char *text;
	void *value;
} Token;

/* Receives an array code lines, it will tokenize the entire array and return a
 * Token array.
 * */
void lex_program(const char *source, const char **program, int length);

/* Lex a single line and return the tokenized values.
 * */
Token *lex_line(const char *source, int row, const char *line,
				int *token_count);

/* Create a new token with the most important information
 * */
void create_token(Token *token, TokenType type, TokenLocation loc, int length,
				  char *text, void *value);

/* Takes a token a return a string representation
 * */
static char *token_string(Token token);

/* As token saves so many things then it's necessary to use this function in
 * order too free that mem at the final of the procecss;
 * */
void free_token(Token *tok);

#endif // LEXER_H
