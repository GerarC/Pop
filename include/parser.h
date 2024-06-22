#ifndef EXPR_H

#define EXPR_H
#include "common.h"

typedef struct _node_t Node;

typedef enum {
	NT_BINARY, // Only has two children (for  math and logical expressions)
	NT_MULTICHILDREN, // has more than two children (for class declarations)
} NodeType;

/* Struct that represents a node in the AST
 * */
typedef struct _node_t {
	Token token;
	NodeType type;
	/*Left node's child*/
	Node *left;
	/*Right node's child*/
	Node *right;
	/*A list of children if tree isn't binary*/
	Node **children;
	/*How many children it has  (only used if node is multi-children)*/
	int child_count;
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

/* Prints an error message in a location of a token
 * */
void print_paser_error(char *message, Token tok);

/* Frees an AST by its root token
 * */
void free_ast(Node *root);

/* Prints the ast from a given root node, receives a level parameter to know
 * which parameter will calculates the number of spaces (starts in zero).
 * */
void print_ast(Node *root, const char *prefix, int is_left, int is_root);

#endif // !EXPR_H
