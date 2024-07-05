#include "../include/semantic.h"
#include "../include/config.h"
#include "../include/log.h"
#include "../include/symboltable.h"
#include <stdlib.h>
#include <string.h>

Scope *semantic_scope;
const char *compare_types(Node *a, Node *b);
void statement_analysis(Node *stmt);
void declaration_analysis(Node *declaration);
void assignment_analysis(Node *assignment);
void ifwhile_analysis(Node *ifwh);
void else_analysis(Node *else_s);
void expression_analysis(Node *expr);
void binaryop_analysis(Node *binary);
void unitary_analysis(Node *unit);
void literal_analysis(Node *lit);

void temp_print_int_analysis(Node *lit);

void semantic_error(char *message, Node *node) {
	Token tok = node->token;
	log_fatal("%s:%i:%i '%s': %s", tok.location.file, tok.location.line,
			  tok.location.col, tok.lexeme, message);
	exit(1);
}

void typing_error(char *message, Node *a, Node *b) {
	Token a_tok = a->token;
	Token b_tok = b->token;
	log_fatal("%s:%i:(%i,%i) '%s'='%s' and '%s'='%s': %s", a_tok.location.file,
			  a_tok.location.line, a_tok.location.col, b_tok.location.col,
			  a_tok.lexeme, a->sem_type, b_tok.lexeme, b->sem_type, message);
	exit(1);
}

void semantic_analysis(Node *ast) {
	log_info("Semantic Analysis starts");
	semantic_scope = create_global_scope();

	if (ast->token.type == TOK_MAIN)
		for (int i = 0; i < ast->child_count; i++) {
			statement_analysis(ast->children[i]);
		}
	else semantic_error("There is not entry point", ast);

	exit_scope(semantic_scope);
	log_info("Semantic Analysis ends");
}

const char *compare_types(Node *a, Node *b) {
	const char *a_type = a->sem_type;
	const char *b_type = b->sem_type;
	// builtin types
	if (strcmp(a_type, b_type) == 0) return a_type;
	else if (strcmp(a_type, "int") == 0 || strcmp(a_type, "float") == 0) {
		if (strcmp(b_type, "int") == 0 || strcmp(b_type, "float") == 0)
			return "float";
		else typing_error("Not compatible types", a, b);
	} else typing_error("Some one is null (a | b)", a, b);
	return NULL;
}

void statement_analysis(Node *stmt) {
	switch (stmt->token.type) {
		// Expressions
		case TOK_IDENTIFIER:
		case TOK_INTTYPE:
		case TOK_BOOLTYPE:
			if (stmt->token.type == TOK_IDENTIFIER && stmt->child_count == 0)
				expression_analysis(stmt);
			else declaration_analysis(stmt);
			break;

		case TOK_ASSIGN:
			assignment_analysis(stmt);
			break;

		case TOK_INT:
		case TOK_FLOAT:
		case TOK_IMAGINARY:
		case TOK_STR:
		case TOK_BOOL:

		case TOK_EQUAL:
		case TOK_DIFF:
		case TOK_NOT:
		case TOK_GT:
		case TOK_GEQT:
		case TOK_LT:
		case TOK_LEQT:
		case TOK_OR:

		case TOK_AND:
		case TOK_XOR:

		case TOK_PLUS:
		case TOK_MINUS:
		case TOK_SLASH:
		case TOK_STAR:
		case TOK_MOD:
		case TOK_BINOR:
		case TOK_BINAND:
		case TOK_BINXOR:
		case TOK_BINNOT:
			expression_analysis(stmt);
			break;

		case TOK_IF:
		case TOK_WHILE:
			ifwhile_analysis(stmt);
			break;

		case TOK_ELSE:
			else_analysis(stmt);
			break;

		case TOK_PRINT_INT:
			temp_print_int_analysis(stmt);
			break;

		default:
			semantic_error("Ilegal Token", stmt);
			break;
	}
}

void declaration_analysis(Node *declaration) {
	const char *type = declaration->token.lexeme;
	/* TODO: there must be a function that take the lexeme of the declaration
	 * and seek in the scope to know if the type exists.
	 * */
	for (int i = 0; i < declaration->child_count; i++) {
		Node *child = declaration->children[i];
		if (child->type == NT_MULTICHILDREN && child->child_count == 0) {
			if (add_symbol(semantic_scope, type, child->token.lexeme) != 0)
				semantic_error("Type doesn't exist", declaration);
			strncpy(declaration->children[i]->sem_type, type, MAX_SYMBOL_SIZE);
		} else {
			if (add_symbol(semantic_scope, type, child->left->token.lexeme))
				semantic_error("Type doesn't exist", declaration);
			strncpy(declaration->children[i]->left->sem_type, type,
					MAX_SYMBOL_SIZE);
			assignment_analysis(child);
		}
	}
}

void assignment_analysis(Node *assignment) {

	if (assignment->right->token.type == TOK_ASSIGN) {
		assignment_analysis(assignment->right);
	} else if (assignment->right->token.type != TOK_INT &&
			   assignment->right->token.type != TOK_BOOL &&
			   assignment->right->token.type != TOK_IDENTIFIER)
		expression_analysis(assignment->right);
	else literal_analysis(assignment->right);

	const char *type =
		find_symbol(semantic_scope, assignment->left->token.lexeme);
	strncpy(assignment->left->sem_type, type, MAX_SYMBOL_SIZE);

	type = compare_types(assignment->left, assignment->right);
	strncpy(assignment->sem_type, type, MAX_SYMBOL_SIZE);
}

void ifwhile_analysis(Node *ifwh) {
	semantic_scope = enter_scope(semantic_scope);
	Token tok = ifwh->token;
	if (tok.type == TOK_IF) {
		expression_analysis(ifwh->children[0]);
		for (int i = 1; i < ifwh->child_count; i++)
			statement_analysis(ifwh->children[i]);
	}
	semantic_scope = exit_scope(semantic_scope);
}

void else_analysis(Node *else_s) {
	Token tok = else_s->token;
	if (tok.type == TOK_IF) {
		for (int i = 0; i < else_s->child_count; i++)
			statement_analysis(else_s->children[i]);
	}
}

void expression_analysis(Node *expr) {
	switch (expr->token.type) {
		case TOK_EQUAL:
		case TOK_DIFF:
		case TOK_GT:
		case TOK_GEQT:
		case TOK_LT:
		case TOK_LEQT:
		case TOK_PLUS:
		case TOK_MINUS:
		case TOK_SLASH:
		case TOK_STAR:
			if (expr->type == NT_BINARY) binaryop_analysis(expr);
			else unitary_analysis(expr);
			break;

		case TOK_NOT:
		case TOK_BINNOT:
			unitary_analysis(expr);
			break;

		case TOK_IDENTIFIER:
		case TOK_INT:
		case TOK_FLOAT:
		case TOK_IMAGINARY:
		case TOK_STR:
		case TOK_BOOL:
			literal_analysis(expr);
			break;

		default:
			semantic_error("Unreacheable expression", expr);
	}
}

void binaryop_analysis(Node *binary) {
	if (binary == NULL) return;
	expression_analysis(binary->left);
	expression_analysis(binary->right);
	if (binary->left != NULL && binary->right != NULL) {
		const char *type = compare_types(binary->left, binary->right);
		switch (binary->token.type) {
			case TOK_EQUAL:
			case TOK_DIFF:
			case TOK_GT:
			case TOK_GEQT:
			case TOK_LT:
			case TOK_LEQT:
				type = "bool";
			default:
				break;
		}
		strncpy(binary->sem_type, type, MAX_SYMBOL_SIZE);
	} else semantic_error("incomplete binary expression", binary);
}

void unitary_analysis(Node *unit) {
	if (unit == NULL) return;
	if (unit->children == NULL)
		semantic_error("Unitary expression incomplete", unit);
	expression_analysis(unit->children[0]);
	const char *type = unit->children[0]->sem_type;
	strncpy(unit->sem_type, type, MAX_SYMBOL_SIZE);
}

void literal_analysis(Node *lit) {
	const char *type = NULL;
	switch (lit->token.type) {

		case TOK_INT:
			strncpy(lit->sem_type, "int", MAX_SYMBOL_SIZE);
			break;

		case TOK_FLOAT:
			strncpy(lit->sem_type, "float", MAX_SYMBOL_SIZE);
			break;

		case TOK_STR:
			strncpy(lit->sem_type, "string", MAX_SYMBOL_SIZE);
			break;

		case TOK_BOOL:
			strncpy(lit->sem_type, "bool", MAX_SYMBOL_SIZE);
			break;

		case TOK_IDENTIFIER:
			type = find_symbol(semantic_scope, lit->token.lexeme);
			strncpy(lit->sem_type, type, MAX_SYMBOL_SIZE);
			break;

		case TOK_IMAGINARY:
		default:
			semantic_error("Unknown type", lit);
			break;
	}
}

void temp_print_int_analysis(Node *print_int) {
	Token tok = print_int->token;
	expression_analysis(print_int->children[0]);
}
