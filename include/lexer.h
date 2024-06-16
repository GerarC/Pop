#ifndef LEXER_H
#define LEXER_H

#include "./common.h"

/* Receives an array code lines, it will tokenize the entire array and return a
 * Token array.
 * */
Token *lex_program(const char *source, const char **program, int length,
				   int *lexer_len);

/* Lex a single line and return the tokenized values.
 * */
Token *lex_line(const char *source, int row, const char *line,
				int *token_count);

int compare_reserved(const char **curr, const char *rword, char **dest,
					 int *col);
/* Free a list of tokens.
 * */
void free_lexer(Token *tokens, int len);

#endif // LEXER_H
