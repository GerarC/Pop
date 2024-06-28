#include "../include/semantic.h"
#include "../include/config.h"
#include "../include/log.h"
#include "../include/symboltable.h"
#include <stdlib.h>
#include <string.h>

static Scope *scope;
const char *compare_types(Node *a, Node *b);
void statement_analysis(Node *stmt);
void expression_analysis(Node *expr);
void binaryop_analysis(Node *binary);
void unitary_analysis(Node *unit);
void literal_analysis(Node *lit);

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
	scope = create_global_scope();

	if (ast->token.type == TOK_MAIN)
		for (int i = 0; i < ast->child_count; i++) {
			statement_analysis(ast->children[i]);
		}
	else semantic_error("There is not entry point", ast);

	exit_scope(scope);
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
	} else semantic_error("Unexpected error", a);
	return NULL;
}

void statement_analysis(Node *stmt) {
	switch (stmt->token.type) {
		// Expressions
		case TOK_SYMBOL:
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
			expression_analysis(stmt);
			break;

		default:
			semantic_error("Ilegal Token", stmt);
			break;
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
			unitary_analysis(expr);
			break;

		case TOK_SYMBOL:
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

		case TOK_IMAGINARY:
		case TOK_SYMBOL:
		default:
			semantic_error("Unknown type", lit);
			break;
	}
}
