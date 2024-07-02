#include "../include/ir.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void program_ir(IntermediateRepresentation *ir, Node *program);
void statement_ir(IntermediateRepresentation *ir, Node *stmt);
void expression_ir(IntermediateRepresentation *ir, Node *expr);
void if_ir(IntermediateRepresentation *ir, Node *expr);
void binaryop_ir(IntermediateRepresentation *ir, Node *bin);
void unitary_ir(IntermediateRepresentation *ir, Node *unit);
void cross_reference_builder(IntermediateRepresentation *ir);
/*void literal_ir(IntermediateRepresentation *ir, Node *lit);*/
IrValue literal_val(IntermediateRepresentation *ir, Node *lit);

char *irval_string(IrValue value) {
	char *string_val = (char *)malloc(sizeof(char) * 10);
	switch (value.type) {
		case IRVAL_INT:
			snprintf(string_val, 10, "%i", value.data.ival);
			break;
		case IRVAL_FLOAT:
			snprintf(string_val, 10, "%.2f", value.data.fval);
			break;
		case IRVAL_ADDRESS:
			snprintf(string_val, 10, "t%u", value.data.index);
			break;
	}
	return string_val;
}

void print_ir(IntermediateRepresentation *ir) {
	if (LOG_DEBUG > LOG_LEVEL) return;
	log_debug("Printing IR");

	for (size i = 0; i < ir->count; i++) {
		IrOperationType type = ir->instructions[i].type;
		char *arg1 = irval_string(ir->instructions[i].arg1);
		char *arg2 = irval_string(ir->instructions[i].arg2);
		char *result = irval_string(ir->instructions[i].result);
		char operation[5];

		switch (type) {
			case IR_ADD:
				strcpy(operation, "+");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_SUB:
				strcpy(operation, "-");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_MUL:
				strcpy(operation, "*");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_DIV:
				strcpy(operation, "/");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;

			case IR_EQUAL:
				strcpy(operation, "==");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_DIFF:
				strcpy(operation, "!=");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_GT:
				strcpy(operation, ">");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_GEQT:
				strcpy(operation, ">=");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_LT:
				strcpy(operation, "<");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;
			case IR_LEQT:
				strcpy(operation, "<=");
				printf("%s = %s %s %s", result, arg1, operation, arg2);
				break;

			case IR_UMINUS:
				strcpy(operation, "-");
				printf("%s = %s %s", result, operation, arg1);
				break;
			case IR_NOT:
				strcpy(operation, "not");
				printf("%s = %s %s", result, operation, arg1);
				break;
			case IR_BINNOT:
				strcpy(operation, "~");
				printf("%s = %s %s", result, operation, arg1);
				break;

			case IR_IF:
				strcpy(operation, "if");
				printf("%s !%s goto endblock_%s", operation, arg1, result);
				break;

			case IR_ENDBLOCK:
				printf("endblock_%s:", result);
				break;

			default:
				log_error("Op %i not implemented", type);
		}
		free(arg1);
		free(arg2);
		printf("\n");
	}
	printf("end\n\n");
}

int is_literal(Node *lit) {
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
	log_warn("PROGRAM IR NOT IMPLEMENTED");
	if (program->token.type != TOK_MAIN) ir_error("Not entry point", program);
	for (size i = 0; i < program->child_count; i++) {
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
		case TOK_BINNOT:
			expression_ir(ir, stmt);
			break;

		case TOK_IF:
			if_ir(ir, stmt);
			break;

		default:
			ir_error("Not implemented", stmt);
			break;
	}
}

void if_ir(IntermediateRepresentation *ir, Node *if_stmt) {
	IrValue arg1 = {0};

	if (if_stmt->children[0]) {
		if (is_literal(if_stmt->children[0])) {
			arg1 = literal_val(ir, if_stmt->children[0]);
		} else {
			expression_ir(ir, if_stmt->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};

	IrOperation op = {.type = IR_IF, .arg1 = arg1, .result = result};

	add_iroperation(ir, op);
	int limit =
		if_stmt->children[if_stmt->child_count - 1]->token.type == TOK_ELSE
			? if_stmt->child_count - 1
			: if_stmt->child_count;

	for (size i = 1; i < limit; i++) {
		statement_ir(ir, if_stmt->children[i]);
	}

	if (limit < if_stmt->child_count)
		log_warn("ELSE INTERMEDIARY REPRESENTATION NOT IMPLEMENTED");

	result.data.index = ir->count;

	IrOperation end_lbl = {.type = IR_ENDBLOCK, .result = result};

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
	log_info("enter to cross reference");
	int stack[MAX_STACK_SIZE];
	int sp = 0;
	for (size i = 0; i < ir->count; i++) {
		IrOperation op = ir->instructions[i];
		if (op.type == IR_IF) {
			log_info("enter to if stack");
			stack[sp] = i;
			sp++;
		} else if (op.type == IR_ENDBLOCK) {
			size index = stack[--sp];
			if (ir->instructions[index].type == IR_IF) {
				ir->instructions[index].result.data.index = i;
			}
			log_info("outs to end stack");
		}
	}
	log_info("outs to cross reference");
	if (sp != 0) {
		log_fatal("NOT ALL BLOCK ARE CLOSED");
		exit(1);
	}
}
