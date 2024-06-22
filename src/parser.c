#include "../include/parser.h"
#include "../include/log.h"
#include <stdlib.h>
#include <string.h>

// I'm doing this to have something graphic and it will be updating
// Parenthesis without quotes are for grouping
/* ### Grammar
 * Program -> statement*
 * statement -> expr tok_eln
 *            | assign tok_eln
 *            | declaration tok_eln
 * expr -> equality
 * equality -> comparison (('=='|'!=') comparison*
 * comparison -> term (('<'|'>'|'<='|'>='|'=='|'!=') term)*
 * term -> factor (('+'|'-') factor)*
 * factor -> literal (('*'|'/'|'%') literal)*
 * literal -> number
 *       | expr
 *       | '(' expr ')'
 * */

/*
 * */
Node *parse_statement(Parser *parser);
Node *parse_expression(Parser *parser);
Node *parse_equality(Parser *parser);
Node *parse_comparison(Parser *parser);
Node *parse_term(Parser *parser);
Node *parse_factor(Parser *parser);
Node *parse_literal(Parser *parser);

/*
 * */
Parser create_parser(Token *tokens, int length) {
	Parser parser = {tokens, length, 0};
	return parser;
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
	Node *statements = (Node *)malloc(sizeof(Node));
	statements->child_count = 0;
	TokenLocation loc = {0, 0, "root"};
	statements->token.type = TOK_MAIN;
	statements->token.location = loc;
	statements->token.length = 0;
	statements->token.value = NULL;
	statements->token.lexeme = NULL;
	statements->type = NT_MULTICHILDREN;
	Node *current = NULL;
	Node **children = NULL;

	while (parser->pos < parser->length &
		   current_token(parser).type != TOK_EOF) {
		log_debug("p:%i\t%s", parser->pos, token_string(current_token(parser)));
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
			print_paser_error("Something wrong happened", statements->token);
			return NULL;
		}

		parser->pos++;
	}
	statements->children = children;

	return statements;
}

Node *parse_statement(Parser *parser) {
	Token tok = current_token(parser);
	Node *stmt = NULL;
	if (tok.type == TOK_LET) stmt = parse_literal(parser);
	else { stmt = parse_expression(parser); }

	log_debug("new line");
	if (current_token(parser).type == TOK_ELN) next(parser);

	print_ast(stmt, "", 0, 1);
	return stmt;
}

Node *parse_expression(Parser *parser) { return parse_equality(parser); }

Node *parse_equality(Parser *parser) {
	Node *left = parse_comparison(parser);
	Token tok = current_token(parser);
	Node *equal = NULL;
	while (tok.type == TOK_EQUAL || tok.type == TOK_DIFF) {
		equal = (Node *)malloc(sizeof(Node));
		equal->token = tok;
		equal->type = NT_BINARY;
		equal->left = left;
		next(parser);
		equal->right = parse_comparison(parser);
		left = equal;
		log_debug("equal:\t%s", token_string(equal->token));
		tok = current_token(parser);
	}
	log_debug("no more comparisons");

	if (equal == NULL) return left;
	return equal;
}

Node *parse_comparison(Parser *parser) {
	Node *left = parse_term(parser);
	Token tok = current_token(parser);
	Node *comp = NULL;
	while (tok.type == TOK_LT || tok.type == TOK_LEQT || tok.type == TOK_GT ||
		   tok.type == TOK_GEQT) {
		comp = (Node *)malloc(sizeof(Node));
		comp->type = NT_BINARY;
		comp->token = tok;
		comp->left = left;
		next(parser);
		comp->right = parse_term(parser);
		left = comp;
		log_debug("comp:\t%s", token_string(comp->token));
		tok = current_token(parser);
	}
	log_debug("no more comparisons");

	if (comp == NULL) return left;
	return comp;
}

Node *parse_term(Parser *parser) {
	Node *left = parse_factor(parser);
	Token tok = current_token(parser);
	Node *term = NULL;
	while (tok.type == TOK_PLUS || tok.type == TOK_MINUS) {
		term = (Node *)malloc(sizeof(Node));
		term->type = NT_BINARY;
		term->token = tok;
		term->left = left;
		next(parser);
		term->right = parse_factor(parser);
		left = term;
		log_debug("term:\t%s", token_string(term->token));
		tok = current_token(parser);
	}
	log_debug("no more terms");
	if (term == NULL) return left;
	return term;
}

Node *parse_factor(Parser *parser) {
	Node *left = parse_literal(parser);
	Token tok = current_token(parser);
	Node *fact = NULL;
	while (tok.type == TOK_SLASH || tok.type == TOK_STAR) {
		fact = (Node *)malloc(sizeof(Node));
		fact->type = NT_BINARY;
		fact->token = tok;
		fact->left = left;
		next(parser);
		fact->right = parse_literal(parser);
		left = fact;
		log_debug("term:\t%s", token_string(fact->token));
		tok = current_token(parser);
	}
	log_debug("no more factors");
	if (fact == NULL) return left;
	return fact;
}

Node *parse_literal(Parser *parser) {
	Token tok = current_token(parser);
	log_debug("lit:\t%s", token_string(current_token(parser)));
	Node *lit = NULL;
	if (tok.type == TOK_NUM || tok.type == TOK_BOOL) {
		lit = (Node *)malloc(sizeof(Node));
		lit->type = NT_BINARY;
		lit->token = tok;
		lit->right = NULL;
		lit->left = NULL;
		next(parser);
	} else if (tok.type == TOK_LPAREN) {
		next(parser);
		lit = parse_comparison(parser);
		;
		if (current_token(parser).type == TOK_RPAREN) next(parser);
		else {
			print_paser_error("A '(' must have a ')", current_token(parser));
			return NULL;
		}
	} else {
		print_paser_error("unexpected token", tok);
		return NULL;
	};

	log_debug("finish");
	return lit;
}

void print_paser_error(char *message, Token tok) {
	TokenLocation loc = tok.location;
	log_error("%s:%i:i %s", loc.file, loc.line, loc.col, message);
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
		for (int i = 0; i < root->child_count; i++)
			free_ast(root->children[i]);

	free(root);
}
