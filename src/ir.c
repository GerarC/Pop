#include "../include/ir.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void program_ir(IntermediateRepresentation *ir, Node *program);
void statement_ir(IntermediateRepresentation *ir, Node *stmt);
void expression_ir(IntermediateRepresentation *ir, Node *expr);
void binaryop_ir(IntermediateRepresentation *ir, Node *bin);
void unitary_ir(IntermediateRepresentation *ir, Node *unit);
/*void literal_ir(IntermediateRepresentation *ir, Node *lit);*/
IrValue literal_val(IntermediateRepresentation *ir, Node *lit);

char *irval_string(IrValue value) {
	char * string_val = (char*) malloc(sizeof(char)*10);
	switch (value.type) {
		case IRVAL_INT:
			snprintf(string_val, 10, "%i", value.data.ival);
			break;
		case IRVAL_FLOAT:
			snprintf(string_val, 10, "%.2f", value.data.fval);
			break;
		case IRVAL_INDEX:
			snprintf(string_val, 10, "(%i)", value.data.index);
			break;
	}
    return string_val;
}

void print_ir(IntermediateRepresentation *ir) {
	if (LOG_DEBUG > LOG_LEVEL) return;
	log_debug("Printing Ir");

	for (int i = 0; i < ir->count; i++) {
		IrOperationType type = ir->instructions[i].type;
        char* arg1 = irval_string(ir->instructions[i].arg1);
        char* arg2 = irval_string(ir->instructions[i].arg2);
		char operation[5];

        printf("(%i):\t", i);
		switch (type) {
			case IR_ADD:
				strcpy(operation, "add");
				printf("%s %s, %s", operation, arg1, arg2);
				break;
			case IR_SUB:
				strcpy(operation, "sub");
				printf("%s %s, %s", operation, arg1, arg2);
				break;
			case IR_MUL:
				strcpy(operation, "mul");
				printf("%s %s, %s", operation, arg1, arg2);
				break;
			case IR_DIV:
				strcpy(operation, "div");
				printf("%s %s, %s", operation, arg1, arg2);
				break;
			default:
				log_error("Op %i not implemented print", type);
		}
        free(arg1);
        free(arg2);
		printf("\n");
	}
}

int is_literal(Node *lit) {
	log_debug("is %s literal?", lit->token.lexeme);
	switch (lit->token.type) {
		case TOK_SYMBOL:
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

	/*ir->intructions[ir->count].arg3 = op.arg3;*/
	/*ir->intructions[ir->count].result = op.result;*/

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

	return ir;
}

void program_ir(IntermediateRepresentation *ir, Node *program) {
	log_warn("PROGRAM IR NOT IMPLEMENTED");
	if (program->token.type != TOK_MAIN) ir_error("Not entry point", program);
	for (int i = 0; i < program->child_count; i++) {
		statement_ir(ir, program->children[i]);
	}
}

void statement_ir(IntermediateRepresentation *ir, Node *stmt) {
	Token tok = stmt->token;
	switch (tok.type) {
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
			expression_ir(ir, stmt);
			break;

		default:
			ir_error("Not implemented", stmt);
			break;
	}
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
			unitary_ir(ir, expr);
			break;

		case TOK_SYMBOL:
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
	log_debug("binary op %s", bin->token.lexeme);
	switch (bin->token.type) {
		case TOK_PLUS:
			op_type = IR_ADD;
			break;

		case TOK_MINUS:
			op_type = IR_SUB;
			break;

		case TOK_SLASH:
			op_type = IR_MUL;
			break;

		case TOK_STAR:
			op_type = IR_DIV;
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
			arg1.type = IRVAL_INDEX;
			arg1.data.index = ir->count - 1;
		}
	}

	if (bin->right != NULL) {
		if (is_literal(bin->right)) {
			arg2 = literal_val(ir, bin->right);
		} else {
			expression_ir(ir, bin->right);
			arg2.type = IRVAL_INDEX;
			arg2.data.index = ir->count - 1;
		}
	}

	IrOperation op = {.type = op_type, .arg1 = arg1, .arg2 = arg2};

	add_iroperation(ir, op);
}

void unitary_ir(IntermediateRepresentation *ir, Node *unit) {
	log_warn("UNITARY OP IR NOT IMPLEMENTED");
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

		default:
			ir_error("literal not implemented ", lit);
			break;
	}
	return value;
}

void free_intermediate_representation(IntermediateRepresentation *ir) {
	log_warn("FREE INTERMEDIARY REPRESENTATION NOT IMPLEMENTED");
}
