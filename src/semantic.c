#include "../include/semantic.h"
#include "../include/config.h"
#include "../include/log.h"
#include "../include/symboltable.h"
#include <stdlib.h>
#include <string.h>

SymbolTable *sem_table = NULL;

const char *compare_types(Node *a, Node *b);
void statement_analysis(Node *stmt);
void declaration_analysis(Node *declaration);
void function_declaration_analysis(Node *fdeclaration);
void function_usage_analysis(Node *funct);
void assignment_analysis(Node *assignment);
void ifwhile_analysis(Node *ifwh);
void block_analysis(Node *block);
void else_analysis(Node *else_s);
void expression_analysis(Node *expr);
void binaryop_analysis(Node *binary);
void unitary_analysis(Node *unit);
void literal_analysis(Node *lit);

void temp_print_int_analysis(Node *lit);
void temp_print_char_analysis(Node *lit);

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

void semantic_analysis(Node *ast, SymbolTable *tbl) {
	log_info("Semantic Analysis");
	sem_table = tbl;
	if (sem_table == NULL) semantic_error("Not semantic table", ast);

	if (ast->token.type == TOK_MAIN)
		for (int i = 0; i < ast->child_count; i++) {
			statement_analysis(ast->children[i]);
		}
	else semantic_error("There is not entry point", ast);
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
	} else if (strcmp(a_type, "int") == 0 || strcmp(a_type, "long") == 0) {
		if (strcmp(b_type, "int") == 0 || strcmp(b_type, "long") == 0)
			return "long";
		else typing_error("Not compatible types", a, b);
	} else typing_error("NOT COMPATIBLE TYPES (a ~ b)", a, b);
	return NULL;
}

void statement_analysis(Node *stmt) {
	switch (stmt->type) {
		// Expressions
		case NT_DECLARATION:
			declaration_analysis(stmt);
			break;

		case NT_FUNC_DECLARATION:
			function_declaration_analysis(stmt);
			break;

		case NT_FUNC_USAGE:
			function_usage_analysis(stmt);
			break;

		case NT_ASSIGNMENT:
			assignment_analysis(stmt);
			break;

		case NT_LITERAL:
		case NT_BINARYOP:
		case NT_UNITARYOP:
			expression_analysis(stmt);
			break;

		case NT_IF:
		case NT_WHILE:
			ifwhile_analysis(stmt);
			break;

		case NT_ELSE:
			else_analysis(stmt);
			break;

		case NT_TEMP_PRINT_INT:
			temp_print_int_analysis(stmt);
			break;

		case NT_TEMP_PRINT_CHAR:
			temp_print_char_analysis(stmt);
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
	Symbol sym;
	for (int i = 0; i < declaration->child_count; i++) {
		Node *child = declaration->children[i];
		if (child->child_count == 0) {
			strncpy(declaration->children[i]->sem_type, type, MAX_SYMBOL_SIZE);
			sym = create_symbol(child, ST_VARIABLE);
			add_symbol(sem_table, sym);
		} else {
			strncpy(declaration->children[i]->children[0]->sem_type, type,
					MAX_SYMBOL_SIZE);
			sym = create_symbol(child->children[0], ST_VARIABLE);
			add_symbol(sem_table, sym);
			assignment_analysis(child);
		}
	}
}

void function_declaration_analysis(Node *fdeclaration) {
	Token tok = fdeclaration->token;
	const char *type = tok.lexeme;
	Symbol sym;
	strncpy(fdeclaration->children[0]->sem_type, type, MAX_SYMBOL_SIZE);
	sym = create_symbol(fdeclaration->children[0], ST_FUNCTION);
	add_symbol(sem_table, sym);
}

void function_usage_analysis(Node *funct) {
	Token tok = funct->token;
	const char *type = tok.lexeme;
	int idx = find_symbol(sem_table, type);
	if (idx == -1) semantic_error("Type doesn't exists", funct);
	print_symbol_table(sem_table);
}

void assignment_analysis(Node *assignment) {
	if (assignment->children[1]->token.type == TOK_ASSIGN) {
		assignment_analysis(assignment->children[1]);
	} else if (assignment->children[1]->token.type != TOK_INT &&
			   assignment->children[1]->token.type != TOK_BOOL &&
			   assignment->children[1]->token.type != TOK_LONG &&
			   assignment->children[1]->token.type != TOK_IDENTIFIER)
		expression_analysis(assignment->children[1]);
	else literal_analysis(assignment->children[1]);

	int idx_type =
		find_symbol(sem_table, assignment->children[0]->token.lexeme);
	const char *type = find_symbol_type(sem_table, idx_type);
	strncpy(assignment->children[0]->sem_type, type, MAX_SYMBOL_SIZE);


	type = compare_types(assignment->children[0], assignment->children[1]);
	strncpy(assignment->sem_type, type, MAX_SYMBOL_SIZE);
}

void ifwhile_analysis(Node *ifwh) {
	Token tok = ifwh->token;
	if (tok.type == TOK_IF) {
		expression_analysis(ifwh->children[0]);
		block_analysis(ifwh->children[1]);
	}
}

void else_analysis(Node *else_s) {
	Token tok = else_s->token;
	if (tok.type == TOK_ELSE) block_analysis(else_s->children[0]);
}

void block_analysis(Node *block) {
	for (int i = 0; i < block->child_count; i++)
		statement_analysis(block->children[i]);
}

void expression_analysis(Node *expr) {
	switch (expr->type) {
		case NT_BINARYOP:
			binaryop_analysis(expr);
			break;

		case NT_UNITARYOP:
			unitary_analysis(expr);
			break;

		case NT_LITERAL:
			literal_analysis(expr);
			break;

		default:
			log_error("error token: %s", token_string(expr->token));
			semantic_error("Unreacheable expression", expr);
	}
}

void binaryop_analysis(Node *binary) {
	if (binary == NULL) return;
	expression_analysis(binary->children[0]);
	expression_analysis(binary->children[1]);
	if (binary->children[0] != NULL && binary->children[1] != NULL) {
		const char *type =
			compare_types(binary->children[0], binary->children[1]);
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
	int idx_type = -1;
	switch (lit->token.type) {

		case TOK_INT:
			strncpy(lit->sem_type, "int", MAX_SYMBOL_SIZE);
			break;

		case TOK_FLOAT:
			strncpy(lit->sem_type, "float", MAX_SYMBOL_SIZE);
			break;

		case TOK_LONG:
			strncpy(lit->sem_type, "long", MAX_SYMBOL_SIZE);
			break;

		case TOK_STR:
			strncpy(lit->sem_type, "string", MAX_SYMBOL_SIZE);
			break;

		case TOK_CHAR:
			strncpy(lit->sem_type, "char", MAX_SYMBOL_SIZE);
			break;

		case TOK_BOOL:
			strncpy(lit->sem_type, "bool", MAX_SYMBOL_SIZE);
			break;

		case TOK_IDENTIFIER:
			idx_type = find_symbol(sem_table, lit->token.lexeme);
			type = find_symbol_type(sem_table, idx_type);
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

void temp_print_char_analysis(Node *print_char) {
	Token tok = print_char->token;
	expression_analysis(print_char->children[0]);
}
