#ifndef LEXER_H
#define LEXER_H

#define MAX_NAME_SIZE 128

typedef enum _token_type_e {
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_DIV,
	TOKEN_MULT,
	TOKEN_NUMBER,
	TOKEN_INVALID
} TokenType;

typedef struct _token_location_t {
	int line, col;
    char file[MAX_NAME_SIZE];
} TokenLocation;

typedef struct _token_t {
	TokenType type;
    TokenLocation location;
} Token;

#endif // LEXER_H
