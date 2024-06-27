#include "../include/parser.h"
#include "../include/log.h"
#include <stdlib.h>
#include <string.h>

/* Returns the current node of the parser
 * */
Token current_token(Parser *parser);

/* Create a new multinode token and sets its children.
 * */
Node *create_mulnode(Token tok, int child_count, Node **children);

/* Create a binary node and sets its left and right children
 * */
Node *create_binnode(Token tok, Node *left, Node *right);

void next(Parser *parser);

/* Prints an error message in a location of a token
 * */
void parser_error(char *message, Token tok);

// I'm doing this to have something graphic and it will be updating
// Parenthesis without quotes are for grouping
/* ### Grammar
 * Program -> statement*
 * statement -> expr tok_eln
 *            | assign tok_eln
 *            | declaration tok_eln
 * expr -> equality
 * equality -> comparison (('=='|'!=') comparison)*
 * comparison -> term (('<'|'>'|'<='|'>='|'=='|'!=') term)*
 * term -> factor (('+'|'-') factor)*
 * factor -> literal (('*'|'/'|'%') literal)*
 * factor -> unary (('*'|'/'|'%') unary)*
 * unary -> ('!'|'-'|'not')?literal
 * literal -> number
 *       | bool
 *       | expr
 *       | '(' expr ')'
 * */

//----------------- Implementation ---------------- //
//
Node *parse_statement(Parser *parser);
Node *parse_expression(Parser *parser);
Node *parse_equality(Parser *parser);
Node *parse_comparison(Parser *parser);
Node *parse_term(Parser *parser);
Node *parse_factor(Parser *parser);
Node *parse_unary(Parser *parser);
Node *parse_literal(Parser *parser);


Parser create_parser(Token *tokens, int length) {
	Parser parser = {tokens, length, 0};
	return parser;
}

Node *create_mulnode(Token tok, int child_count, Node **children) {
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	node->token = tok;
	node->type = NT_MULTICHILDREN;
	node->child_count = child_count;
	node->children = children;
	node->left = NULL;
	node->right = NULL;
	return node;
}

Node *create_binnode(Token tok, Node *left, Node *right) {
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	node->token = tok;
	node->type = NT_BINARY;
	node->left = left;
	node->right = right;
	node->child_count = 0;
	return node;
}

Token current_token(Parser *parser) {
	if (parser->pos < parser->length) return parser->tokens[parser->pos];
	else {
		Token tok;
		tok.type = TOK_EOF;
		return tok;
	}
}

void next(Parser *parser) {
	if (parser->pos < parser->length) parser->pos++;
}

Node *parse_program(Parser *parser) {
	log_info("Parsing Tokens");
	TokenLocation loc = {0, 0, "root"};
	Token tok = {TOK_MAIN, loc, 0, NULL, NULL};

	Node *statements = create_mulnode(tok, 0, NULL);
	Node *current = NULL;
	Node **children = NULL;

	while (parser->pos < parser->length &
		   current_token(parser).type != TOK_EOF) {
		current = parse_statement(parser);
		if (current != NULL) {
			if (statements->child_count == 0) {
				statements->child_count++;
				children = (Node **)malloc(sizeof(Node *));
			} else {
				statements->child_count++;
				children = (Node **)realloc(
					children, sizeof(Node *) * statements->child_count);
			}
			children[statements->child_count - 1] = current;
		} else {
			parser_error("Something wrong happened", statements->token);
			return NULL;
		}
	}
	statements->children = children;

	return statements;
}

Node *parse_statement(Parser *parser) {
	Token tok = current_token(parser);
	Node *stmt = NULL;
	if (tok.type == TOK_LET) stmt = parse_literal(parser);
	else { stmt = parse_expression(parser); }

	if (current_token(parser).type == TOK_ELN) next(parser);

	return stmt;
}

Node *parse_expression(Parser *parser) { return parse_equality(parser); }

Node *parse_equality(Parser *parser) {
	Node *left = parse_comparison(parser);
	Token tok = current_token(parser);
	Node *equal = NULL;
	while (tok.type == TOK_EQUAL || tok.type == TOK_DIFF) {
		next(parser);
		equal = create_binnode(tok, left, parse_comparison(parser));
		left = equal;
		tok = current_token(parser);
	}

	if (equal == NULL) return left;
	return equal;
}

Node *parse_comparison(Parser *parser) {
	Node *left = parse_term(parser);
	Token tok = current_token(parser);
	Node *comp = NULL;

	while (tok.type == TOK_LT || tok.type == TOK_LEQT || tok.type == TOK_GT ||
		   tok.type == TOK_GEQT) {
		next(parser);
		comp = create_binnode(tok, left, parse_term(parser));
		left = comp;
		tok = current_token(parser);
	}
	if (comp == NULL) return left;
	return comp;
}

Node *parse_term(Parser *parser) {
	Node *left = parse_factor(parser);
	Token tok = current_token(parser);
	Node *term = NULL;
	while (tok.type == TOK_PLUS || tok.type == TOK_MINUS) {
		next(parser);
		term = create_binnode(tok, left, parse_factor(parser));
		left = term;
		tok = current_token(parser);
	}
	if (term == NULL) return left;
	return term;
}

Node *parse_factor(Parser *parser) {
	Node *left = parse_unary(parser);
	Token tok = current_token(parser);
	Node *fact = NULL;
	int enters_loop = 0;
	while (tok.type == TOK_SLASH || tok.type == TOK_STAR) {
		next(parser);
		fact = create_binnode(tok, left, parse_unary(parser));
		left = fact;
		tok = current_token(parser);
		enters_loop = 1;
	}
	if (fact == NULL) return left;
	return fact;
}

Node *parse_unary(Parser *parser) {
	Token tok = current_token(parser);
	Node *unary = NULL;
	if (tok.type == TOK_MINUS || tok.type == TOK_NOT) {
		next(parser);
		unary = create_mulnode(tok, 1, NULL);
		unary->children = (Node **)malloc(sizeof(Node *));
		unary->children[0] = parse_literal(parser);
	} else {
		return parse_literal(parser);
	}

	return unary;
}

Node *parse_literal(Parser *parser) {
	Token tok = current_token(parser);
	Node *lit = NULL;
	if (tok.type == TOK_INT || tok.type == TOK_FLOAT || tok.type == TOK_BOOL || tok.type == TOK_SYMBOL) {
		lit = create_mulnode(tok, 0, NULL);
		next(parser);
	} else if (tok.type == TOK_LPAREN) {
		next(parser);
		lit = parse_equality(parser);

		if (current_token(parser).type == TOK_RPAREN) next(parser);
		else
			parser_error("An ') before the end of the line expected",
						 current_token(parser));

	} else parser_error("unexpected token", tok);

	return lit;
}

void parser_error(char *message, Token tok) {
	log_fatal("%s:%i:%i '%s': %s", tok.location.file, tok.location.line,
			  tok.location.col, tok.lexeme, message);
	exit(1);
}

void print_ast(Node *root, const char *prefix, int is_left, int is_root) {
	if (root == NULL) return;
	char new_prefix[1024];

	if (!is_root) {
		printf("%s", prefix);
		printf("%s", is_left ? "├─ " : "└─ ");
		strcpy(new_prefix, prefix);
		strcat(new_prefix, is_left ? "│  " : "   ");
	} else new_prefix[0] = '\0';

	if (root->token.type == TOK_MAIN) printf(".\n");
	else if (root->token.lexeme == NULL) printf("null\n");
	else printf("%s\n", root->token.lexeme);

	if (root->type == NT_BINARY) {
		print_ast(root->left, new_prefix, 1, 0);
		print_ast(root->right, new_prefix, 0, 0);
	} else {
		int not_final = 1;
		for (int i = 0; i < root->child_count; i++) {
			if (i == root->child_count - 1) not_final = 0;
			print_ast(root->children[i], new_prefix, not_final, 0);
		}
	}
}

void free_ast(Node *root) {

	if (root->right != NULL) free_ast(root->right);
	if (root->left != NULL) free_ast(root->left);
	if (root->type == NT_MULTICHILDREN && root->child_count > 0)
		for (int i = 0; i < root->child_count; i++) {
			Node *temp = root->children[i];
			free_ast(temp);
		}

	free(root);
}
