#ifndef EXPR_H
#define EXPR_H
#include "common.h"

typedef struct _node_t Node;

typedef struct _node_t {
	Token token;
	Node *left;
	Node *right;
} Node;

typedef struct _parser_t {
	Token *tokens;
	int length;
	int pos;
} Parser;

Parser create_parser(Token *tokens, int length);

Token current_token(Parser *parser);

void next(Parser *parser);

Node *parse_program(Parser *parser);

#endif // !EXPR_H
