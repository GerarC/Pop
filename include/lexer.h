#ifndef LEXER_H
#define LEXER_H

#include "./config.h"
#include "./token.h"
#include "./common.h"

typedef struct _lexer_t {
	Token *tokens;
	char source[MAX_FILE_SIZE];
	int count;
	int capacity;
} Lexer;

/* creates a lexer struct which manages the token list
 * */
Lexer create_lexer(const char *source);

/* malloc's a new token with give data and adds it to the parser
 * */
void add_token(Lexer *lexer, TokenType type, SourceLocation location, int length,
			   char *lexeme, void *value);

/* Receives an array code lines, it will tokenize the entire array and return a
 * Token array.
 * */
void lex_program(Lexer *lexer, const char *program);

/* lex a work a sets its size and the destination
 * */
int is_reser(const char **curr, const char *rword, char **dest,
					 int *col);

/* Takes a lexer and print its token list
 * */
void print_lexer(Lexer *lex);

/* Free a list of tokens.
 * */
void free_lexer(Lexer lex);

#endif // LEXER_H
