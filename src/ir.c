#include "../include/ir.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>

void program_ir(IntermediateRepresentation *ir, Node *program);
void statement_ir(IntermediateRepresentation *ir, Node *stmt);
void expression_ir(IntermediateRepresentation *ir, Node *expr);
void binaryop_ir(IntermediateRepresentation *ir, Node *bin);
void unitary_ir(IntermediateRepresentation *ir, Node *unit);
/*void literal_ir(IntermediateRepresentation *ir, Node *lit);*/
IrValue literal_val(IntermediateRepresentation *ir, Node *lit);

void print_ir(IntermediateRepresentation *ir) {
	for (int i = 0; i < ir->count; i++) {
		int arg1 = ir->intructions[i].arg1.data.i_val;
		int arg2 = ir->intructions[i].arg2.data.i_val;
		int arg3 = ir->intructions[i].arg3.data.i_val;
		int result = ir->intructions[i].result.data.i_val;
		log_trace("Op {type: %zu, arg1: %i, arg2: %i, arg3: %i, result: %i}",
				  ir->intructions[i].type, arg1, arg2, arg3, result);
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
		ir->intructions =
			(IrOperation *)realloc(ir->intructions, sizeof(IrOperation) * ir->capacity);
	}
	ir->intructions[ir->count].type = op.type;
	ir->intructions[ir->count].arg1 = op.arg1;
	log_info("final %i , %i", op.arg2.type, op.arg2.data.i_val);
	ir->intructions[ir->count].arg2 = op.arg2;
	ir->intructions[ir->count].arg3 = op.arg3;
	ir->intructions[ir->count].result = op.result;
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
	ir->intructions = (IrOperation *)malloc(sizeof(IrOperation));
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
	log_trace("hello");
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
			log_info("literal %i , %i", arg1.type, arg1.data.i_val);
		} else {
			expression_ir(ir, bin->left);
			arg1 = ir->intructions[ir->count - 1].result;
		}
		log_debug("%s left %i", bin->token.lexeme, arg1.data.i_val);
	}

	if (bin->right != NULL) {
		if (is_literal(bin->right)) {
			arg2 = literal_val(ir, bin->right);
			log_info("literal %i , %i", arg2.type, arg2.data.i_val);
		} else {
			expression_ir(ir, bin->right);
			arg2 = ir->intructions[ir->count - 1].result;
		}
		log_debug("%s right %i", bin->token.lexeme, arg2.data.i_val);
	}

	IrValue result = {0};
	result.type = IRVAL_INT;

	IrOperation op = {
		.type = op_type, .arg1 = arg1, .arg2 = arg2, .result = result};

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
			value.data.i_val = *ival;
			break;
		case TOK_FLOAT:
			value.type = IRVAL_FLOAT;
			float *fval = lit->token.value;
			value.data.f_val = *fval;
			break;

		default:
			ir_error("literal not implemented ", lit);
			break;
	}
	log_info("literal %i , %i", value.type, value.data.i_val);
	return value;
}

void free_intermediate_representation(IntermediateRepresentation *ir) {
	log_warn("FREE INTERMEDIARY REPRESENTATION NOT IMPLEMENTED");
}
