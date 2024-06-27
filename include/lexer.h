#ifndef LEXER_H
#define LEXER_H

#include "./token.h"

/* Receives an array code lines, it will tokenize the entire array and return a
 * Token array.
 * */
Token *lex_program(const char *source, const char *program, int *lexer_len);

/* Free a list of tokens.
 * */
void free_lexer(Token *tokens, int len);

#endif // LEXER_H
