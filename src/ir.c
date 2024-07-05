#include "../include/ir.h"
#include "../include/log.h"
#include "../include/symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Scope *scope = NULL;

void program_ir(IntermediateRepresentation *ir, Node *program);
void statement_ir(IntermediateRepresentation *ir, Node *stmt);

void declaration_ir(IntermediateRepresentation *ir, Node *declaration);
void assignment_ir(IntermediateRepresentation *ir, Node *assignment);

void if_ir(IntermediateRepresentation *ir, Node *expr);
void else_ir(IntermediateRepresentation *ir, Node *else_s);
void while_ir(IntermediateRepresentation *ir, Node *while_s);

void expression_ir(IntermediateRepresentation *ir, Node *expr);
void binaryop_ir(IntermediateRepresentation *ir, Node *bin);
void unitary_ir(IntermediateRepresentation *ir, Node *unit);
void cross_reference_builder(IntermediateRepresentation *ir);
/*void literal_ir(IntermediateRepresentation *ir, Node *lit);*/
IrValue literal_val(IntermediateRepresentation *ir, Node *lit);

void temp_print_int_ir(IntermediateRepresentation *ir, Node *print_int);

char *irval_string(IrValue value) {
	char *string_val = (char *)malloc(sizeof(char) * MAX_SYMBOL_SIZE);
	switch (value.type) {
		case IRVAL_INT:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%i", value.data.ival);
			break;
		case IRVAL_FLOAT:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%.2f", value.data.fval);
			break;
		case IRVAL_ADDRESS:
			snprintf(string_val, MAX_SYMBOL_SIZE, "t%u", value.data.index);
			break;
		case IRVAL_IDENTIFIER:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%s", value.data.ident);
			break;
	}
	return string_val;
}

void print_ir(IntermediateRepresentation *ir) {
	if (LOG_DEBUG < LOG_LEVEL) return;
	log_debug("Printing IR");

	printf("begin\n");
	for (size i = 0; i < ir->count; i++) {
		IrOperationType type = ir->instructions[i].type;
		char *arg1 = irval_string(ir->instructions[i].arg1);
		char *arg2 = irval_string(ir->instructions[i].arg2);
		char *result = irval_string(ir->instructions[i].result);
		char operation[16];

		switch (type) {
			case IR_DECLARATION:
				strcpy(operation, "create");
				printf("\t%s %s", operation, arg1);
				break;
			case IR_ASSIGNMENT:
				strcpy(operation, "=");
				printf("\t%s %s %s", arg1, operation, arg2);
				break;

			case IR_IF:
				strcpy(operation, "if");
				printf("%s !%s goto endblock_%s", operation, arg1, result);
				break;
			case IR_ELSE:
				strcpy(operation, "else");
				printf("%s", operation);
				break;
			case IR_WHILE:
				strcpy(operation, "while");
				printf("%s", operation);
				break;
			case IR_DO:
				strcpy(operation, "do");
				printf("%s %s", arg1, operation);
				break;

			case IR_ADD:
				strcpy(operation, "+");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_SUB:
				strcpy(operation, "-");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_MUL:
				strcpy(operation, "*");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_DIV:
				strcpy(operation, "/");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;

			case IR_EQUAL:
				strcpy(operation, "==");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_DIFF:
				strcpy(operation, "!=");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_GT:
				strcpy(operation, ">");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_GEQT:
				strcpy(operation, ">=");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_LT:
				strcpy(operation, "<");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_LEQT:
				strcpy(operation, "<=");
				printf("\t%s = %s %s %s", result, arg1, operation, arg2);
				break;

			case IR_UMINUS:
				strcpy(operation, "-");
				printf("\t%s = %s %s", result, operation, arg1);
				break;
			case IR_NOT:
				strcpy(operation, "not");
				printf("\t%s = %s %s", result, operation, arg1);
				break;
			case IR_BINNOT:
				strcpy(operation, "~");
				printf("\t%s = %s %s", result, operation, arg1);
				break;

			case IR_TEMP_PRINT_INT:
				strcpy(operation, "print");
				printf("\t%s(%s)", operation, arg1);
				break;

			case IR_ENDBLOCK:
				printf("endblock_%s:", result);
				break;

			default:
				log_error("Op %i printing not implemented", type);
		}
		free(arg1);
		free(arg2);
		printf("\n");
	}
	printf("end\n\n");
}

int is_literal(Node *lit) {
	switch (lit->token.type) {
		case TOK_IDENTIFIER:
		case TOK_INT:
		case TOK_FLOAT:
		case TOK_IMAGINARY:
		case TOK_STR:
		case TOK_BOOL:
			return 1;
			break;
		default:
			return 0;
			break;
	}
}

void add_iroperation(IntermediateRepresentation *ir, IrOperation op) {
	if (ir->count >= ir->capacity) {
		ir->capacity *= 2;
		ir->instructions = (IrOperation *)realloc(
			ir->instructions, sizeof(IrOperation) * ir->capacity);
	}
	ir->instructions[ir->count].type = op.type;
	ir->instructions[ir->count].arg1 = op.arg1;
	ir->instructions[ir->count].arg2 = op.arg2;
	ir->instructions[ir->count].result = op.result;

	ir->count++;
}

void ir_error(const char *message, Node *node) {
	Token tok = node->token;
	log_fatal("%s:%i:%i %s '%s': %s", tok.location.file, tok.location.line,
			  tok.location.col, node->sem_type, tok.lexeme, message);
	exit(1);
}

IntermediateRepresentation *create_intermediate_representation(Node *ast) {
	IntermediateRepresentation *ir = (IntermediateRepresentation *)malloc(
		sizeof(IntermediateRepresentation));
	ir->instructions = (IrOperation *)malloc(sizeof(IrOperation));
	ir->count = 0;
	ir->capacity = 1;

	program_ir(ir, ast);

	cross_reference_builder(ir);

	return ir;
}

void program_ir(IntermediateRepresentation *ir, Node *program) {
	scope = create_global_scope();
	if (program->token.type != TOK_MAIN) ir_error("Not entry point", program);
	for (size i = 0; i < program->child_count; i++) {
		statement_ir(ir, program->children[i]);
	}
	exit_scope(scope);
}

void statement_ir(IntermediateRepresentation *ir, Node *stmt) {
	Token tok = stmt->token;
	switch (tok.type) {
		case TOK_IDENTIFIER:
		case TOK_BOOLTYPE:
		case TOK_INTTYPE:
			declaration_ir(ir, stmt);
			break;

		case TOK_ASSIGN:
			assignment_ir(ir, stmt);
			break;

		case TOK_IF:
			if_ir(ir, stmt);
			break;

		case TOK_WHILE:
			while_ir(ir, stmt);
			break;

		case TOK_ELSE:
			else_ir(ir, stmt);
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
			expression_ir(ir, stmt);
			break;

		case TOK_PRINT_INT:
			temp_print_int_ir(ir, stmt);
			break;

		default:
			ir_error("Not implemented", stmt);
			break;
	}
}

void declaration_ir(IntermediateRepresentation *ir, Node *declaration) {
	IrValue arg1 = {IRVAL_IDENTIFIER};
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_DECLARATION, .result = result};
	add_entry(scope, declaration->sem_type);
	for (int i = 0; i < declaration->child_count; i++) {
		Node *child = declaration->children[i];
		if (child->type == NT_MULTICHILDREN) {
			add_symbol(scope, declaration->token.lexeme, child->token.lexeme);
			strncpy(arg1.data.ident, child->token.lexeme, MAX_SYMBOL_SIZE);
			arg1 = literal_val(ir, child);
			op.arg1 = arg1;
			add_iroperation(ir, op);
		} else {
			add_symbol(scope, declaration->token.lexeme,
					   child->left->token.lexeme);
			strncpy(arg1.data.ident, child->left->token.lexeme,
					MAX_SYMBOL_SIZE);
			arg1 = literal_val(ir, child->left);
			op.arg1 = arg1;
			add_iroperation(ir, op);
			assignment_ir(ir, child);
		}
	}
}

void assignment_ir(IntermediateRepresentation *ir, Node *assignment) {
	IrValue arg2;
	if (assignment->right->token.type == TOK_ASSIGN) {
		assignment_ir(ir, assignment->right);
		arg2.type = IRVAL_ADDRESS;
		arg2.data.index = ir->count - 1;
	} else if (assignment->right->token.type != TOK_INT &&
			   assignment->right->token.type != TOK_BOOL &&
			   assignment->right->token.type != TOK_IDENTIFIER) {
		expression_ir(ir, assignment->right);
		arg2.type = IRVAL_ADDRESS;
		arg2.data.index = ir->count - 1;
	} else arg2 = literal_val(ir, assignment->right);

	Node *destination = assignment->left;
	IrValue arg1 = literal_val(ir, assignment->left);
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {IR_ASSIGNMENT, arg1, arg2, result};
	add_iroperation(ir, op);
}

void if_ir(IntermediateRepresentation *ir, Node *if_stmt) {
	IrValue arg1 = {0};

	if (if_stmt->children[0]) {
		if (is_literal(if_stmt->children[0]))
			arg1 = literal_val(ir, if_stmt->children[0]);
		else {
			expression_ir(ir, if_stmt->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_IF, .arg1 = arg1, .result = result};
	add_iroperation(ir, op);

	for (size i = 1; i < if_stmt->child_count; i++)
		statement_ir(ir, if_stmt->children[i]);

	result.data.index = ir->count;

	IrOperation end_lbl = {
		.type = IR_ENDBLOCK, .arg1 = result, .result = result};

	add_iroperation(ir, end_lbl);
}

void else_ir(IntermediateRepresentation *ir, Node *else_s) {
	if (ir->instructions[ir->count - 1].type != IR_ENDBLOCK)
		ir_error("An else block must be after another block", else_s);
	ir->instructions[ir->count - 1].type = IR_ELSE;

	for (size i = 0; i < else_s->child_count; i++)
		statement_ir(ir, else_s->children[i]);

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation end_lbl = {.type = IR_ENDBLOCK, .result = result};
	add_iroperation(ir, end_lbl);
}

void while_ir(IntermediateRepresentation *ir, Node *while_s) {
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation while_op = {.type = IR_WHILE, .result = result};
	add_iroperation(ir, while_op);

	IrValue arg1 = {0};
	if (while_s->children[0]) {
		if (is_literal(while_s->children[0]))
			arg1 = literal_val(ir, while_s->children[0]);
		else {
			expression_ir(ir, while_s->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	result.data.index = ir->count;
	IrOperation op = {.type = IR_DO, .arg1 = arg1, .result = result};
	add_iroperation(ir, op);

	for (size i = 1; i < while_s->child_count; i++)
		statement_ir(ir, while_s->children[i]);

	result.data.index = ir->count;

	IrOperation end_lbl = {
		.type = IR_ENDBLOCK, .arg1 = result, .result = result};

	add_iroperation(ir, end_lbl);
}

void expression_ir(IntermediateRepresentation *ir, Node *expr) {
	Token tok = expr->token;
	switch (tok.type) {
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
			if (expr->type == NT_BINARY) binaryop_ir(ir, expr);
			else unitary_ir(ir, expr);
			break;

		case TOK_NOT:
		case TOK_BINNOT:
			unitary_ir(ir, expr);
			break;

		case TOK_IDENTIFIER:
		case TOK_INT:
		case TOK_FLOAT:
		case TOK_IMAGINARY:
		case TOK_STR:
		case TOK_BOOL:
			literal_val(ir, expr);
			break;

		default:
			ir_error("Unreacheable ", expr);
			break;
	}
}

void binaryop_ir(IntermediateRepresentation *ir, Node *bin) {
	IrOperationType op_type;
	switch (bin->token.type) {
		case TOK_PLUS:
			op_type = IR_ADD;
			break;
		case TOK_MINUS:
			op_type = IR_SUB;
			break;
		case TOK_STAR:
			op_type = IR_MUL;
			break;
		case TOK_SLASH:
			op_type = IR_DIV;
			break;
		case TOK_EQUAL:
			op_type = IR_EQUAL;
			break;
		case TOK_DIFF:
			op_type = IR_DIFF;
			break;
		case TOK_GT:
			op_type = IR_GT;
			break;
		case TOK_GEQT:
			op_type = IR_GEQT;
			break;
		case TOK_LT:
			op_type = IR_LT;
			break;
		case TOK_LEQT:
			op_type = IR_LEQT;
			break;

		default:
			ir_error("Binary operation not implemented ", bin);
			break;
	}
	IrValue arg1 = {0};
	IrValue arg2 = {0};

	if (bin->left != NULL) {
		if (is_literal(bin->left)) {
			arg1 = literal_val(ir, bin->left);
		} else {
			expression_ir(ir, bin->left);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	if (bin->right != NULL) {
		if (is_literal(bin->right)) {
			arg2 = literal_val(ir, bin->right);
		} else {
			expression_ir(ir, bin->right);
			arg2.type = IRVAL_ADDRESS;
			arg2.data.index = ir->count - 1;
		}
	}
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};

	IrOperation op = {
		.type = op_type, .arg1 = arg1, .arg2 = arg2, .result = result};

	add_iroperation(ir, op);
}

void unitary_ir(IntermediateRepresentation *ir, Node *unit) {
	IrOperationType op_type;
	switch (unit->token.type) {
		case TOK_MINUS:
			op_type = IR_UMINUS;
			break;

		case TOK_NOT:
			op_type = IR_NOT;
			break;

		case TOK_BINNOT:
			op_type = IR_BINNOT;
			break;

		default:
			ir_error("NOT REACHABLE UNITARY OPERATION", unit);
			break;
	}
	IrValue arg1 = {0};
	if (is_literal(unit->children[0]))
		arg1 = literal_val(ir, unit->children[0]);
	else {
		expression_ir(ir, unit->children[0]);
		arg1.type = IRVAL_ADDRESS;
		arg1.data.index = ir->count - 1;
	}
	int res_addr = arg1.type == IRVAL_ADDRESS ? arg1.data.index : ir->count;

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = res_addr};
	IrOperation op = {.type = op_type, .arg1 = arg1, .result = result};

	add_iroperation(ir, op);
}

IrValue literal_val(IntermediateRepresentation *ir, Node *lit) {
	IrValue value = {0};
	switch (lit->token.type) {
		case TOK_INT:
			value.type = IRVAL_INT;
			int *ival = lit->token.value;
			value.data.ival = *ival;
			break;
		case TOK_FLOAT:
			value.type = IRVAL_FLOAT;
			float *fval = lit->token.value;
			value.data.fval = *fval;
			break;

		case TOK_IDENTIFIER:
			value.type = IRVAL_IDENTIFIER;
			char *sval = lit->token.lexeme;
			if (find_symbol(scope, sval) == NULL)
				ir_error("Symbol doesn't exists", lit);
			strncpy(value.data.ident, sval, MAX_SYMBOL_SIZE);
			break;

		default:
			ir_error("literal not implemented ", lit);
			break;
	}
	return value;
}

void free_intermediate_representation(IntermediateRepresentation *ir) {
	log_warn("FREE INTERMEDIARY REPRESENTATION NOT IMPLEMENTED");
}

void cross_reference_builder(IntermediateRepresentation *ir) {
	int stack[MAX_STACK_SIZE];
	int sp = 0;
	for (size i = 0; i < ir->count; i++) {
		IrOperation op = ir->instructions[i];
		if (op.type == IR_IF || op.type == IR_WHILE || op.type == IR_DO) {
			stack[sp] = i;
			sp++;
		} else if (op.type == IR_ELSE) {
			size index = stack[--sp];
			ir->instructions[index].result.data.index = i;
			stack[sp] = i;
			sp++;
		} else if (op.type == IR_ENDBLOCK) {
			size index = stack[--sp];
			ir->instructions[i].arg1.type = IRVAL_INT;
			if (ir->instructions[index].type == IR_IF)
				ir->instructions[index].result.data.index = i;
			else if (ir->instructions[index].type == IR_DO) {
				ir->instructions[index].result.data.index = i;
				index = stack[--sp];
				if (ir->instructions[index].type == IR_WHILE) {
					ir->instructions[i].arg1.type = IRVAL_ADDRESS;
					ir->instructions[i].arg1.data.index = index;
				}
			}
		}
	}
	if (sp != 0) {
		log_fatal("NOT ALL BLOCK ARE CLOSED");
		exit(1);
	}
}

void temp_print_int_ir(IntermediateRepresentation *ir, Node *print_int) {
	IrValue arg1 = {0};

	if (print_int->children[0]) {
		if (is_literal(print_int->children[0])) {
			arg1 = literal_val(ir, print_int->children[0]);
		} else {
			expression_ir(ir, print_int->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};

	IrOperation op = {
		.type = IR_TEMP_PRINT_INT, .arg1 = arg1, .result = result};

	add_iroperation(ir, op);
}
