#ifndef EXPR_H

#define EXPR_H
#include "config.h"
#include "lexer.h"
#include "token.h"
#include "typedef.h"

typedef struct _node_t Node;

typedef enum {
	NT_MAIN,
	NT_TYPE,
	NT_LITERAL,
	NT_IDENTIFIER,

	NT_BINARYOP,
	NT_UNITARYOP,
	NT_ASSIGNMENT,
	NT_DECLARATION,

	NT_IF,
	NT_WHILE,
	NT_ELSE,
	NT_BLOCK_STATEMENTS,

	NT_FUNC_DECLARATION,
	NT_FUNC_ARGUMENT_DECL,
    NT_TEMP_PRINT_INT,
    NT_TEMP_PRINT_CHAR,
} NodeType;

/* Struct that represents a node in the AST
 * */
typedef struct _node_t {
	Token token;
	NodeType type;
	/*Left node's child*/
	Node **children;
	/*How many children it has*/
	int child_count;
	int capacity;
	/*Type for the semantic analysis*/
	char sem_type[MAX_SYMBOL_SIZE];
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

void print_ast(Node *root);

/* Frees an AST by its root token
 * */
void free_ast(Node *root);

#endif // !EXPR_H
