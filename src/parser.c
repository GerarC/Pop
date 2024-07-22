#include "../include/parser.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: CREATE NODE GRAMMAR TYPES IS NEEDED
/* Returns the current node of the parser
 * */
Token current_token(Parser *parser);

/* Create a new multinode token and sets its children.
 * */
Node *create_ast_node(Token tok, NodeType type);

void add_child(Node *parent, Node *child);

void peek(Parser *parser);

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
 *          | function_usage
 *          | while_stmt
 *
 *
 * declaration -> type idents tok_eln
 *          | type identifier '(' func_decl (',' func_decl)* ')' block_statement
 * type -> 'int'
 *       | 'bool'
 *       | identifier
 * idents -> identifier (',' idents)*
 *       | assign (',' idents)*
 * assign -> identifier '=' assign*
 *       | expr tok_eln
 *
 * if_stmt -> 'if' '('expr')' block_statements else_stmt? tok_eln
 * else_stmt -> else block_statements
 * while_stmt -> 'while' '('expr')' block_statements
 *
 * block_statements -> '{'statement+'}' tok_eln
 *       | '('expr')' statement tok_eln
 *
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
Node *parse_variable_declaration(Parser *parser);
Node *parse_type(Parser *parser);
Node *parse_idents(Parser *parser);
Node *parse_assign(Parser *parser);
Node *parse_function_declaration(Parser *parser);
Node *parse_function_arguments(Parser *parser);
Node *parse_function_usage(Parser *parser);
Node *parse_function_usage_arguments(Parser *parser);

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

Node *get_block_statements(Parser *parser);
Node *parse_temp_print_int(Parser *parser);
Node *parse_temp_print_char(Parser *parser);

Parser create_parser(Lexer *lex) {
	Parser parser = {.tokens = lex->tokens, lex->count, 0};
	return parser;
}

Node *create_ast_node(Token tok, NodeType type) {
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	node->token = tok;
	node->child_count = 0;
	node->capacity = 1;
	node->children = (Node **)malloc(sizeof(Node *));
	node->type = type;
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
		case TOK_VOIDTYPE:
		case TOK_IDENTIFIER:
			return 1;

		default:
			return 0;
	}
}

void peek(Parser *parser) {
	if (parser->pos < parser->length) parser->pos++;
}

Node *parse_program(Parser *parser) {
	log_info("Syntactic analysis");
	Token tok = {TOK_MAIN, parser->tokens[0].location, 1, "main .", NULL};

	Node *current = NULL;
	Node *statements = create_ast_node(tok, NT_MAIN);

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
	else if (tok.type == TOK_IDENTIFIER &&
			 next_token(parser).type == TOK_LPAREN)
		stmt = parse_function_usage(parser);
	else if (tok.type == TOK_ELSE) stmt = parse_else(parser);
	else if (tok.type == TOK_PRINT_INT) stmt = parse_temp_print_int(parser);
	else if (tok.type == TOK_PRINT_CHAR) stmt = parse_temp_print_char(parser);
	else if (tok.type == TOK_IDENTIFIER &&
			 next_token(parser).type == TOK_ASSIGN)
		stmt = parse_assign(parser);
	else if (is_type(tok)) stmt = parse_declaration(parser);
	else { stmt = parse_expression(parser); }

	if (current_token(parser).type == TOK_ELN) peek(parser);

	return stmt;
}

Node *parse_declaration(Parser *parser) {
	if (next_token(parser).type == TOK_IDENTIFIER &&
		parser->tokens[parser->pos + 2].type == TOK_LPAREN)
		return parse_function_declaration(parser);
	else if (next_token(parser).type == TOK_IDENTIFIER)
		return parse_variable_declaration(parser);
	parser_error("An identifier expected, this was given:", next_token(parser));
	return NULL;
}

Node *parse_variable_declaration(Parser *parser) {
	Token tok = current_token(parser);
	Node *declaration = create_ast_node(tok, NT_DECLARATION);
	Node *current = NULL;
	peek(parser);
	while (current_token(parser).type != TOK_ELN) {
		current = parse_idents(parser);
		if (current != NULL) add_child(declaration, current);
		else parser_error("Error parsing declaration of %s", tok);
		if (current_token(parser).type == TOK_COMMA) peek(parser);
		else if (current_token(parser).type != TOK_ELN)
			parser_error("There must be a comma", current_token(parser));
	}

	return declaration;
}

Node *parse_function_declaration(Parser *parser) {
	Token tok = current_token(parser);
	Node *declaration = create_ast_node(tok, NT_FUNC_DECLARATION);
	Node *current = NULL;
	peek(parser);
	Node *func_name = parse_literal(parser);
	if (func_name != NULL) add_child(declaration, func_name);
	if (current_token(parser).type == TOK_LPAREN) {
		parse_function_arguments(parser);
		peek(parser);
		if (current_token(parser).type == TOK_RPAREN) peek(parser);
		else
			parser_error("A ')' expected in function declaration",
						 next_token(parser));
	}
	Node *block = get_block_statements(parser);
	add_child(declaration, block);

	return declaration;
}

Node *parse_function_arguments(Parser *parser) {
	log_warn("Parse Function Arguments not implemented");
	return NULL;
}

Node *parse_function_usage(Parser *parser) {
	Token tok = current_token(parser);
	Node *usage = create_ast_node(tok, NT_FUNC_USAGE);
	Node *current = NULL;
	peek(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		parse_function_usage_arguments(parser);
		peek(parser);
		if (current_token(parser).type == TOK_RPAREN) peek(parser);
		else
			parser_error("A ')' expected in function declaration",
						 next_token(parser));
	}

	return usage;
}

Node *parse_function_usage_arguments(Parser *parser) {
	log_warn("Parse Function Usage Arguments not implemented");
	return NULL;
}

Node *parse_idents(Parser *parser) {
	Token tok = current_token(parser);
	if (next_token(parser).type == TOK_ASSIGN) return parse_assign(parser);
	Node *identifier = create_ast_node(tok, NT_IDENTIFIER);
	peek(parser);
	return identifier;
}

Node *parse_assign(Parser *parser) {
	Node *left = NULL;
	Node *right = NULL;
	Token tok = current_token(parser);
	if (next_token(parser).type != TOK_ELN &&
		next_token(parser).type != TOK_COMMA &&
		next_token(parser).type != TOK_ASSIGN) {
		left = parse_expression(parser);
	} else if (tok.type == TOK_INT || tok.type == TOK_FLOAT ||
			   tok.type == TOK_BOOL || tok.type == TOK_CHAR ||
			   tok.type == TOK_IDENTIFIER)
		left = create_ast_node(tok, NT_LITERAL);
	else left = create_ast_node(tok, NT_IDENTIFIER);
	peek(parser);

	tok = current_token(parser);
	Node *assign = NULL;

	while (tok.type == TOK_ASSIGN) {
		peek(parser);
		assign = create_ast_node(tok, NT_ASSIGNMENT);
		add_child(assign, left);
		right = parse_assign(parser);
		if (right != NULL) add_child(assign, right);
		else parser_error("Error creating right child", tok);
		left = assign;
		tok = current_token(parser);
	}

	if (assign == NULL) return left;
	return assign;
}

Node *parse_ifwhile(Parser *parser) {
	Token tok = current_token(parser);
	Node *stmt = NULL;
	Node *current = NULL;
	peek(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		peek(parser);
		if (tok.type == TOK_IF) stmt = create_ast_node(tok, NT_IF);
		else if (tok.type == TOK_WHILE) stmt = create_ast_node(tok, NT_WHILE);

		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) peek(parser);
		else
			parser_error("A ')' expected in  if or while statement",
						 current_token(parser));

		add_child(stmt, current);
	}

	Node *block = get_block_statements(parser);
	add_child(stmt, block);

	return stmt;
}

Node *parse_else(Parser *parser) {
	int else_if = 0;
	Token tok = current_token(parser);
	Node *else_stmt = NULL;

	else_stmt = create_ast_node(tok, NT_ELSE);

	peek(parser);
	if (current_token(parser).type == TOK_IF) else_if = 1;
	Node *block = get_block_statements(parser);
	add_child(else_stmt, block);
	if (else_if && current_token(parser).type == TOK_ELSE) {
		Node *elif = parse_else(parser);
		add_child(block, elif);
	}

	return else_stmt;
}

Node *parse_expression(Parser *parser) { return parse_equality(parser); }

Node *parse_equality(Parser *parser) {
	Node *left = parse_comparison(parser);
	Token tok = current_token(parser);
	Node *equal = NULL;
	Node *right = NULL;
	while (tok.type == TOK_EQUAL || tok.type == TOK_DIFF) {
		peek(parser);

		equal = create_ast_node(tok, NT_BINARYOP);
		add_child(equal, left);
		right = parse_comparison(parser);
		if (right != NULL) add_child(equal, right);
		else parser_error("Error creating right child", tok);

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
	Node *right = NULL;

	while (tok.type == TOK_LT || tok.type == TOK_LEQT || tok.type == TOK_GT ||
		   tok.type == TOK_GEQT) {
		peek(parser);

		comp = create_ast_node(tok, NT_BINARYOP);
		add_child(comp, left);
		right = parse_term(parser);
		if (right != NULL) add_child(comp, right);
		else parser_error("Error creating right child", tok);

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
	Node *right = NULL;

	while (tok.type == TOK_PLUS || tok.type == TOK_MINUS) {
		peek(parser);

		term = create_ast_node(tok, NT_BINARYOP);
		add_child(term, left);
		right = parse_factor(parser);
		if (right != NULL) add_child(term, right);
		else parser_error("Error creating right child", tok);

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
	Node *right = NULL;
	while (tok.type == TOK_SLASH || tok.type == TOK_STAR ||
		   tok.type == TOK_MOD) {
		peek(parser);

		fact = create_ast_node(tok, NT_BINARYOP);
		add_child(fact, left);
		right = parse_unary(parser);
		if (right != NULL) add_child(fact, right);
		else parser_error("Error creating right child", tok);

		left = fact;
		tok = current_token(parser);
	}
	if (fact == NULL) return left;
	return fact;
}

Node *parse_unary(Parser *parser) {
	Token tok = current_token(parser);
	Node *unary = NULL;
	if (tok.type == TOK_MINUS || tok.type == TOK_NOT ||
		tok.type == TOK_BINNOT) {
		peek(parser);
		unary = create_ast_node(tok, NT_UNITARYOP);
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
		lit = create_ast_node(tok, NT_LITERAL);
		peek(parser);
	} else if (tok.type == TOK_LPAREN) {
		peek(parser);
		lit = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) peek(parser);
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
	int not_final = 1;

	if (!is_root) {
		printf("%s", prefix);
		printf("%s", is_left ? "├─ " : "└─ ");
		strcpy(new_prefix, prefix);
		strcat(new_prefix, is_left ? "│  " : "   ");
	} else {
		new_prefix[0] = '\0';
	}

	if (root->token.type == TOK_MAIN) printf(".\n");
	else if (root->token.type == TOK_BLOCK_GLUE) printf("_\n");
	else if (root->token.lexeme == NULL) printf("null\n");
	else printf("%s\n", root->token.lexeme);

	for (int i = 0; i < root->child_count; i++) {
		if (i == root->child_count - 1) not_final = 0;
		print_helper(root->children[i], new_prefix, not_final, 0);
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
	for (int i = 0; i < root->child_count; i++) {
		Node *temp = root->children[i];
		if (temp != NULL) free_ast(temp);
	}
	/*free(root->children);*/

	free(root);
}

Node *get_block_statements(Parser *parser) {
	Token tok = {TOK_BLOCK_GLUE, current_token(parser).location, 0, "glue",
				 NULL};
	Node *block_stmt = create_ast_node(tok, NT_BLOCK_STATEMENTS);
	Node *current = NULL;
	if (current_token(parser).type == TOK_LCURLY) {
		peek(parser);
		if (current_token(parser).type == TOK_ELN) peek(parser);
		while (parser->pos < parser->length &&
			   current_token(parser).type != TOK_RCURLY) {
			current = parse_statement(parser);
			if (current != NULL) add_child(block_stmt, current);
			else parser_error("Creating statement", current_token(parser));
		}

		if (current_token(parser).type == TOK_RCURLY) peek(parser);
		else parser_error("A '}' expected", current_token(parser));

	} else {
		if (current_token(parser).type == TOK_ELN) peek(parser);
		current = parse_statement(parser);
		if (current != NULL) add_child(block_stmt, current);
		else parser_error("Error creating statement", current_token(parser));
	}
	return block_stmt;
}

Node *parse_temp_print_int(Parser *parser) {
	// WARNING: This is a temporal function, must and is going to be deleted
	Token tok = current_token(parser);
	Node *print_stmt = NULL;
	Node *current = NULL;
	peek(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		peek(parser);
		print_stmt = create_ast_node(tok, NT_TEMP_PRINT_INT);
		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) peek(parser);
		else parser_error("A ')' expected in print int", current_token(parser));
		add_child(print_stmt, current);
	}
	return print_stmt;
}

Node *parse_temp_print_char(Parser *parser) {
	// WARNING: This is a temporal function, must and is going to be deleted
	Token tok = current_token(parser);
	Node *print_stmt = NULL;
	Node *current = NULL;
	peek(parser);
	if (current_token(parser).type == TOK_LPAREN) {
		peek(parser);
		print_stmt = create_ast_node(tok, NT_TEMP_PRINT_CHAR);
		current = parse_expression(parser);

		if (current_token(parser).type == TOK_RPAREN) peek(parser);
		else
			parser_error("A ')' expected int print char",
						 current_token(parser));
		add_child(print_stmt, current);
	}
	return print_stmt;
}
