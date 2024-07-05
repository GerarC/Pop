#ifndef EXPR_H

#define EXPR_H
#include "token.h"
#include "lexer.h"
#include "typedef.h"
#include "config.h"

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
	/*How many children it has  (only used if node is multi-children)*/
	int child_count;
	/*Left node's child*/
	Node *left;
	/*Right node's child*/
	Node *right;
	/*A list of children if tree isn't binary*/
	Node **children;
    /*Type for the semantic analysis*/
    char sem_type[MAX_SYMBOL_SIZE];
    int capacity;
} Node;

/* Struct that points tokens and helps in the syntax analysis labor
 * */
typedef struct _parser_t {
	Token *tokens;
	size length;
	size pos;
} Parser;

/* Receives a token list (returned in the lexing step) and its size, will be
 * used to create the AST.
 * */
Parser create_parser(Lexer *lex);

/* Takes the parser and create an AST of the program
 * */
Node *parse_program(Parser *parser);

void print_ast(Node * root);

/* Frees an AST by its root token
 * */
void free_ast(Node *root);

#endif // !EXPR_H
