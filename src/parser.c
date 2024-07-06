#include "../include/parser.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns the current node of the parser
 * */
Token current_token(Parser *parser);

/* Create a new multinode token and sets its children.
 * */
Node *create_mulnode(Token tok);

void add_child(Node *parent, Node *child);

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
 *          | declaration
 *          | assignments
 *          | if_stmt
 *
 *
 * declaration -> type idents tok_eln
 * type -> 'int'
 *       | 'bool'
 *       | identifier
 * idents -> identifier (',' idents)*
 *       | assign (',' idents)*
 * assign -> identifier '=' assign*
 *       | expr tok_eln
 *
 * if_stmt -> 'if' '('expr')' '{'statement+'}' else_stmt tok_eln
 *       | 'if' '('expr')' statement+ tok_eln
 * else_stmt -> else '{'statement+'}' tok_eln
 *       | else statement tok_eln
 *
 * expr -> equality
 * equality -> comparison (('=='|'!=') comparison)*
 * comparison -> term (('<'|'>'|'<='|'>='|'=='|'!=') term)*
 * term -> factor (('+'|'-') factor)*
 * factor -> literal (('*'|'/'|'%') literal)*
 * factor -> unary (('*'|'/'|'%') unary)*
 * unary -> ('!'|'-'|'not')?literal
 * literal -> int
 *       | bool
 *       | symbol
 *       | expr
 *       | '(' expr ')'
 * */

//----------------- Implementation ---------------- //
//
Node *parse_statement(Parser *parser);

Node *parse_declaration(Parser *parser);
Node *parse_type(Parser *parser);
Node *parse_idents(Parser *parser);
Node *parse_assign(Parser *parser);

Node *parse_ifwhile(Parser *parser);
Node *parse_else(Parser *parser);
Node *parse_while(Parser *parser);

Node *parse_expression(Parser *parser);
Node *parse_equality(Parser *parser);
Node *parse_comparison(Parser *parser);
Node *parse_term(Parser *parser);
Node *parse_factor(Parser *parser);
Node *parse_unary(Parser *parser);
Node *parse_literal(Parser *parser);

void get_block_statements(Parser *parser, Node **parent);
Node *parse_temp_print_int(Parser *parser);
Node *parse_temp_print_char(Parser *parser);

Parser create_parser(Lexer *lex) {
	Parser parser = {.tokens = lex->tokens, lex->count, 0};
	return parser;
}

Node *create_mulnode(Token tok) {
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	node->token = tok;
	node->type = NT_MULTICHILDREN;
	node->child_count = 0;
	node->capacity = 1;
	node->children = (Node **)malloc(sizeof(Node *));
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
	node->capacity = 0;
	return node;
}

void add_child(Node *parent, Node *child) {
	if (parent == NULL) {
		log_fatal("parent must exists");
		exit(1);
	}
	if (child == NULL) parser_error("Child of %s must exists", child->token);

	if (parent->child_count >= parent->capacity) {
		parent->capacity *= 2;
		parent->children = (Node **)realloc(parent->children,
											sizeof(Node *) * parent->capacity);
	}
	parent->children[parent->child_count] = child;
	parent->child_count++;
}

Token current_token(Parser *parser) {
	if (parser->pos < parser->length) return parser->tokens[parser->pos];
	else {
		Token tok;
		tok.type = TOK_EOF;
		return tok;
	}
}

Token next_token(Parser *parser) {
	if (parser->pos + 1 < parser->length)
		return parser->tokens[parser->pos + 1];
	else {
		Token tok;
		tok.type = TOK_EOF;
		return tok;
	}
}

int is_type(Token tok) {
	switch (tok.type) {
		case TOK_INTTYPE:
		case TOK_BOOLTYPE:
		case TOK_CHARTYPE:
		case TOK_IDENTIFIER:
			return 1;

		default:
			return 0;
	}
}

void next(Parser *parser) {
	if (parser->pos < parser->length) parser->pos++;
}

Node *parse_program(Parser *parser) {
	log_info("Syntactic analysis");
	Token tok = {TOK_MAIN, parser->tokens[0].location, 1, "main .", NULL};

	Node *current = NULL;
	Node *statements = create_mulnode(tok);

	while (parser->pos < parser->length &
		   current_token(parser).type != TOK_EOF) {
		current = parse_statement(parser);
		if (current != NULL) {
			add_child(statements, current);
		} else {
			parser_error("Error parsing statement at program",
						 statements->token);
		}
	}

	return statements;
}

Node *parse_statement(Parser *parser) {
	Token tok = current_token(parser);
	Node *stmt = NULL;
	if (tok.type == TOK_LET) stmt = parse_literal(parser);
	else if (tok.type == TOK_IF || tok.type == TOK_WHILE)
		stmt = parse_ifwhile(parser);
	else if (tok.type == TOK_ELSE) stmt = parse_else(parser);
	else if (tok.type == TOK_PRINT_INT) stmt = parse_temp_print_int(parser);
	else if (tok.type == TOK_PRINT_CHAR) stmt = parse_temp_print_char(parser);
	else if (is_type(tok) && next_token(parser).type == TOK_IDENTIFIER)
		stmt = parse_declaration(parser);
	else if (tok.type == TOK_IDENTIFIER &&
			 next_token(parser).type == TOK_ASSIGN)
		stmt = parse_assign(parser);
	else { stmt = parse_expression(parser); }

	if (current_token(parser).type == TOK_ELN) next(parser);

	return stmt;
}

Node *parse_declaration(Parser *parser) {
	Token tok = current_token(parser);
	Node *declaration = create_mulnode(tok);
	Node *current = NULL;
	Node **children = NULL;
	next(parser);
	while (current_token(parser).type != TOK_ELN) {
		current = parse_idents(parser);
		if (current != NULL) {
			add_child(declaration, current);
		} else parser_error("Error parsing declaration of %s", tok);
		if (current_token(parser).type == TOK_COMMA) next(parser);
		else if (current_token(parser).type != TOK_ELN)
			parser_error("There must be a comma", current_token(parser));
	}

	return declaration;
}

Node *parse_idents(Parser *parser) {
	Token tok = current_token(parser);
	if (next_token(parser).type == TOK_ASSIGN) return parse_assign(parser);
	Node *identifier = create_mulnode(tok);
	next(parser);
	return identifier;
}

Node *parse_assign(Parser *parser) {
	Node *left = NULL;
	Token tok = current_token(parser);
	if (next_token(parser).type != TOK_ELN &&
		next_token(parser).type != TOK_COMMA &&
		next_token(parser).type != TOK_ASSIGN)
		left = parse_expression(parser);
	else left = create_mulnode(tok);
	next(parser);

	tok = current_token(parser);
	Node *assign = NULL;

	while (tok.type == TOK_ASSIGN) {
		next(parser);
		assign = create_binnode(tok, left, parse_assign(parser));
		left = assign;
		tok = current_token(parser);
	}

	if (assign == NULL) return left;
	return assign;
}

Node *parse_ifwhile(Parser *parser) {
	Token tok = current_token(parser);
	Node *if_stmt = NULL;
	Node *current = NULL;
	next(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		next(parser);
		if_stmt = create_mulnode(tok);
		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) next(parser);
		else parser_error("A ')' expected", current_token(parser));

		add_child(if_stmt, current);
	}

	get_block_statements(parser, &if_stmt);

	return if_stmt;
}

Node *parse_else(Parser *parser) {
	Token tok = current_token(parser);
	Node *else_stmt = NULL;
	Node **children = NULL;

	else_stmt = create_mulnode(tok);

	next(parser);
	get_block_statements(parser, &else_stmt);

	return else_stmt;
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
	if (tok.type == TOK_MINUS || tok.type == TOK_NOT ||
		tok.type == TOK_BINNOT) {
		next(parser);
		unary = create_mulnode(tok);
		unary->children = (Node **)malloc(sizeof(Node *));
		add_child(unary, parse_expression(parser));
	} else {
		return parse_literal(parser);
	}

	return unary;
}

Node *parse_literal(Parser *parser) {
	Token tok = current_token(parser);
	Node *lit = NULL;
	if (tok.type == TOK_INT || tok.type == TOK_FLOAT || tok.type == TOK_BOOL ||
		tok.type == TOK_CHAR || tok.type == TOK_IDENTIFIER) {
		lit = create_mulnode(tok);
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

/* Prints the given AST, Receives the root node, a prefix used in the printing
 * algorithm (it should be ab empty string), set a 0 to the 'is_left' and
 * finally set as true 'is_root'.
 * */
void print_helper(Node *root, const char *prefix, int is_left, int is_root) {
	char new_prefix[1024];

	if (!is_root) {
		printf("%s", prefix);
		printf("%s", is_left ? "├─ " : "└─ ");
		strcpy(new_prefix, prefix);
		strcat(new_prefix, is_left ? "│  " : "   ");
	} else {
		new_prefix[0] = '\0';
	}

	if (root->token.type == TOK_MAIN) printf(".\n");
	else if (root->token.lexeme == NULL) printf("null\n");
	else printf("%s\n", root->token.lexeme);

	if (root->type == NT_BINARY) {
		print_helper(root->left, new_prefix, 1, 0);
		print_helper(root->right, new_prefix, 0, 0);
	} else {
		int not_final = 1;
		for (int i = 0; i < root->child_count; i++) {
			if (i == root->child_count - 1) not_final = 0;
			print_helper(root->children[i], new_prefix, not_final, 0);
		}
	}
}

void print_ast(Node *root) {
	if (LOG_DEBUG < LOG_LEVEL) return;
	log_debug("Print AST");
	if (root == NULL) return;
	print_helper(root, "", 0, 1);
	printf("\n");
}

void free_ast(Node *root) {
	if (root == NULL) return;
	if (root->right != NULL) free_ast(root->right);
	if (root->left != NULL) free_ast(root->left);
	if (root->type == NT_MULTICHILDREN && root->child_count > 0) {
		for (int i = 0; i < root->child_count; i++) {
			Node *temp = root->children[i];
			free_ast(temp);
		}
		free(root->children);
	}

	free(root);
}

void get_block_statements(Parser *parser, Node **parent) {
	Node *current = NULL;
	if (current_token(parser).type == TOK_LCURLY) {
		next(parser);
		if (current_token(parser).type == TOK_ELN) next(parser);
		while (parser->pos < parser->length &&
			   current_token(parser).type != TOK_RCURLY) {
			current = parse_statement(parser);
			if (current != NULL) add_child(*parent, current);
			else parser_error("Creating statement", current_token(parser));
		}

		if (current_token(parser).type == TOK_RCURLY) next(parser);
		else parser_error("A '}' expected", current_token(parser));

	} else {
		if (current_token(parser).type == TOK_ELN) next(parser);
		current = parse_statement(parser);
		if (current != NULL) add_child(*parent, current);
		else parser_error("Error creating statement", current_token(parser));
	}
}

Node *parse_temp_print_int(Parser *parser) {
	// WARNING: This is a temporal function, must and is going to be deleted
	Token tok = current_token(parser);
	Node *print_stmt = NULL;
	Node *current = NULL;
	next(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		next(parser);
		print_stmt = create_mulnode(tok);
		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) next(parser);
		else parser_error("A ')' expected", current_token(parser));
		add_child(print_stmt, current);
	}
	return print_stmt;
}

Node *parse_temp_print_char(Parser *parser) {
	// WARNING: This is a temporal function, must and is going to be deleted
	Token tok = current_token(parser);
	Node *print_stmt = NULL;
	Node *current = NULL;
	next(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		next(parser);
		print_stmt = create_mulnode(tok);
		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) next(parser);
		else parser_error("A ')' expected", current_token(parser));
		add_child(print_stmt, current);
	}
	return print_stmt;
}
