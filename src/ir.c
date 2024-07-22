#include "../include/ir.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable *ir_table = NULL;
void program_ir(IntermediateRepresentation *ir, Node *program);
void statement_ir(IntermediateRepresentation *ir, Node *stmt);

void func_declaration_ir(IntermediateRepresentation *ir, Node *declaration);
void func_usage_ir(IntermediateRepresentation *ir, Node *declaration);
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
void temp_print_char_ir(IntermediateRepresentation *ir, Node *print_char);

char *irval_string(IrValue value);
void print_ir(IntermediateRepresentation *ir);
int is_literal(Node *lit);
void add_iroperation(IntermediateRepresentation *ir, IrOperation op);
void ir_error(const char *message, Node *node);

void program_ir(IntermediateRepresentation *ir, Node *program) {
	log_info("Intermediate Representation creation");
	if (program->token.type != TOK_MAIN) ir_error("Not entry point", program);
	for (size i = 0; i < program->child_count; i++) {
		statement_ir(ir, program->children[i]);
	}
}

void statement_ir(IntermediateRepresentation *ir, Node *stmt) {
	switch (stmt->type) {
		case NT_FUNC_DECLARATION:
			func_declaration_ir(ir, stmt);
			break;

		case NT_FUNC_USAGE:
			func_usage_ir(ir, stmt);
			break;

		case NT_DECLARATION:
			declaration_ir(ir, stmt);
			break;

		case NT_ASSIGNMENT:
			assignment_ir(ir, stmt);
			break;

		case NT_IF:
			if_ir(ir, stmt);
			break;

		case NT_WHILE:
			while_ir(ir, stmt);
			break;

		case NT_ELSE:
			else_ir(ir, stmt);
			break;

		case NT_LITERAL:

		case NT_BINARYOP:
		case NT_UNITARYOP:
			expression_ir(ir, stmt);
			break;

		case NT_TEMP_PRINT_INT:
			temp_print_int_ir(ir, stmt);
			break;

		case NT_TEMP_PRINT_CHAR:
			temp_print_char_ir(ir, stmt);
			break;

		default:
			ir_error("Not implemented", stmt);
			break;
	}
}

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
		case IRVAL_CHAR:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%c", value.data.cval);
			break;
	}
	return string_val;
}

void print_ir(IntermediateRepresentation *ir) {
	if (LOG_DEBUG < LOG_LEVEL) return;
	log_debug("Printing IR");

	printf("begin\n");
	for (size i = 0; i < ir->count; i++) {
		printf("%i\t", i);
		IrOperationType type = ir->instructions[i].type;
		char *arg1 = irval_string(ir->instructions[i].arg1);
		char *arg2 = irval_string(ir->instructions[i].arg2);
		char *result = irval_string(ir->instructions[i].result);
		const char *data_type;
		char operation[16];

		switch (type) {
			case IR_FUNCT_DECLARATION:
			case IR_DECLARATION:
				strcpy(operation, "create");
				printf("\t%s %s", operation, arg1);
				break;

			case IR_FUNCT_END:
				strcpy(operation, "end func");
				printf("\t%s", operation);
				break;

			case IR_FUNCT_USAGE:
				strcpy(operation, "using");
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
			case IR_MOD:
				strcpy(operation, "%");
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
			case IR_TEMP_PRINT_CHAR:
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
		case TOK_CHAR:
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
	if (node != NULL) {
		Token tok = node->token;
		log_fatal("%s:%i:%i %s '%s': %s", tok.location.file, tok.location.line,
				  tok.location.col, node->sem_type, tok.lexeme, message);
	} else log_fatal("%s", message);
	exit(1);
}

IntermediateRepresentation *create_ir() {
	IntermediateRepresentation *ir = (IntermediateRepresentation *)malloc(
		sizeof(IntermediateRepresentation));
	ir->instructions = (IrOperation *)malloc(sizeof(IrOperation));
	ir->count = 0;
	ir->capacity = 1;
	return ir;
}

IntermediateRepresentation *
create_intermediate_representation(Node *ast, SymbolTable *tbl) {
	IntermediateRepresentation *ir = create_ir();
	ir->globals = create_ir();
	ir_table = tbl;

	program_ir(ir, ast);

	cross_reference_builder(ir);
	cross_reference_builder(ir->globals);

	return ir;
}

void declaration_ir(IntermediateRepresentation *ir, Node *declaration) {
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_DECLARATION, .result = result};
	Symbol sym;
	for (int i = 0; i < declaration->child_count; i++) {
		Node *child = declaration->children[i];
		if (child->type == NT_LITERAL || child->type == NT_IDENTIFIER) {
			op.arg1 = literal_val(ir->globals, child);
			strncpy(op.arg1.data.ident, child->token.lexeme, MAX_SYMBOL_SIZE);

			// Search if it's a local variable or a global one
			int idx = find_symbol(ir_table, op.arg1.data.ident);
			sym = get_symbol(ir_table, idx);
			if (sym.scope == SC_GLOBAL) add_iroperation(ir->globals, op);
			else add_iroperation(ir, op);

		} else {
			op.arg1 = literal_val(ir, child->children[0]);
			strncpy(op.arg1.data.ident, child->children[0]->token.lexeme,
					MAX_SYMBOL_SIZE);

			// Search if it's a local variable or a global one
			int idx = find_symbol(ir_table, op.arg1.data.ident);
			sym = get_symbol(ir_table, idx);
			if (sym.scope == SC_GLOBAL) add_iroperation(ir->globals, op);
			else add_iroperation(ir, op);

			assignment_ir(ir, child);
		}
	}
}

void func_declaration_ir(IntermediateRepresentation *ir, Node *function) {
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_FUNCT_DECLARATION, .result = result};
	Node *child = function->children[0];
	op.arg1 = literal_val(ir, child);
	strncpy(op.arg1.data.ident, child->token.lexeme, MAX_SYMBOL_SIZE);

	// Search if it's a local variable or a global one
	int idx = find_symbol(ir_table, child->token.lexeme);
	Symbol sym = get_symbol(ir_table, idx);
	if (sym.scope == SC_GLOBAL) add_iroperation(ir->globals, op);
	else add_iroperation(ir, op);

	Node *block = function->children[1];
	for (size i = 0; i < block->child_count; i++)
		statement_ir(ir->globals, block->children[i]);

	result.data.index = ir->count;
	op.type = IR_FUNCT_END;
	op.result = result;
	add_iroperation(ir->globals, op);
}
void func_usage_ir(IntermediateRepresentation *ir, Node *function) {
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_FUNCT_USAGE, .result = result};
	op.arg1.type = IRVAL_IDENTIFIER;
	strncpy(op.arg1.data.ident, function->token.lexeme, MAX_SYMBOL_SIZE);
	add_iroperation(ir, op);
}

void assignment_ir(IntermediateRepresentation *ir, Node *assignment) {
	IrValue arg2;
	if (assignment->children[1]->token.type == TOK_ASSIGN) {
		assignment_ir(ir, assignment->children[1]);
		ir->instructions[ir->count - 1].is_used = 1;
		arg2.type = IRVAL_ADDRESS;
		arg2.data.index = ir->count - 1;
	} else if (assignment->children[1]->token.type != TOK_INT &&
			   assignment->children[1]->token.type != TOK_BOOL &&
			   assignment->children[1]->token.type != TOK_CHAR &&
			   assignment->children[1]->token.type != TOK_STR &&
			   assignment->children[1]->token.type != TOK_IDENTIFIER) {
		expression_ir(ir, assignment->children[1]);
		arg2.type = IRVAL_ADDRESS;
		arg2.data.index = ir->count - 1;
	} else arg2 = literal_val(ir, assignment->children[1]);

	Node *destination = assignment->children[0];
	IrValue arg1 = literal_val(ir, assignment->children[0]);
	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {IR_ASSIGNMENT, arg1, arg2, result};
	op.is_used = 0;
	add_iroperation(ir, op);
}

void if_ir(IntermediateRepresentation *ir, Node *if_s) {
	IrValue arg1 = {0};

	if (if_s->children[0]) {
		if (is_literal(if_s->children[0]))
			arg1 = literal_val(ir, if_s->children[0]);
		else {
			expression_ir(ir, if_s->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};
	IrOperation op = {.type = IR_IF, .arg1 = arg1, .result = result};
	add_iroperation(ir, op);

	Node *block = if_s->children[1];
	for (size i = 0; i < block->child_count; i++)
		statement_ir(ir, block->children[i]);

	result.data.index = ir->count;

	IrOperation end_lbl = {
		.type = IR_ENDBLOCK, .arg1 = result, .result = result};

	add_iroperation(ir, end_lbl);
}

void else_ir(IntermediateRepresentation *ir, Node *else_s) {
	if (ir->instructions[ir->count - 1].type != IR_ENDBLOCK)
		ir_error("An else block must be after another block", else_s);
	ir->instructions[ir->count - 1].type = IR_ELSE;

	Node *block = else_s->children[0];
	for (size i = 0; i < block->child_count; i++)
		statement_ir(ir, block->children[i]);

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

	Node *block = while_s->children[1];
	for (size i = 0; i < block->child_count; i++)
		statement_ir(ir, block->children[i]);

	result.data.index = ir->count;

	IrOperation end_lbl = {
		.type = IR_ENDBLOCK, .arg1 = result, .result = result};

	add_iroperation(ir, end_lbl);
}

void expression_ir(IntermediateRepresentation *ir, Node *expr) {
	switch (expr->type) {
		case NT_BINARYOP:
			binaryop_ir(ir, expr);
			break;

		case NT_UNITARYOP:
			unitary_ir(ir, expr);
			break;

		case NT_LITERAL:
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
		case TOK_MOD:
			op_type = IR_MOD;
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

	if (bin->children[0] != NULL) {
		if (is_literal(bin->children[0])) {
			arg1 = literal_val(ir, bin->children[0]);
		} else {
			expression_ir(ir, bin->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	if (bin->children[1] != NULL) {
		if (is_literal(bin->children[1])) {
			arg2 = literal_val(ir, bin->children[1]);
		} else {
			expression_ir(ir, bin->children[1]);
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

		case TOK_CHAR:
			value.type = IRVAL_CHAR;
			value.data.cval = lit->token.lexeme[0];
			break;
		case TOK_FLOAT:
			value.type = IRVAL_FLOAT;
			float *fval = lit->token.value;
			value.data.fval = *fval;
			break;

		case TOK_BOOL:
			value.type = IRVAL_INT;
			if (strcmp(lit->token.lexeme, "false") == 0) value.data.ival = 0;
			else value.data.ival = 1;
			break;
			break;

		case TOK_IDENTIFIER:
			value.type = IRVAL_IDENTIFIER;
			char *sval = lit->token.lexeme;
			strncpy(value.data.ident, sval, MAX_SYMBOL_SIZE);
			break;

		default:
			ir_error("literal not implemented ", lit);
			break;
	}
	return value;
}

void free_intermediate_representation(IntermediateRepresentation *ir) {
	if (ir->globals != NULL) free_intermediate_representation(ir->globals);
	free(ir->instructions);
	/*free(ir);*/
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
			if (ir->instructions[index].type == IR_IF ||
				ir->instructions[index].type == IR_ELSE)
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
	if (sp != 0) ir_error("NOT ALL BLOCK ARE CLOSED", NULL);
}

void temp_print_int_ir(IntermediateRepresentation *ir, Node *print_int) {
	// WARN: This functions is temporal
	IrValue arg1 = {0};
	if (print_int->children[0]) {
		if (is_literal(print_int->children[0]))
			arg1 = literal_val(ir, print_int->children[0]);
		else {
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

void temp_print_char_ir(IntermediateRepresentation *ir, Node *print_char) {
	// WARN: This functions is temporal
	IrValue arg1 = {0};
	if (print_char->children[0]) {
		if (is_literal(print_char->children[0]))
			arg1 = literal_val(ir, print_char->children[0]);
		else {
			expression_ir(ir, print_char->children[0]);
			arg1.type = IRVAL_ADDRESS;
			arg1.data.index = ir->count - 1;
		}
	}

	IrValue result = {.type = IRVAL_ADDRESS, .data.index = ir->count};

	IrOperation op = {
		.type = IR_TEMP_PRINT_CHAR, .arg1 = arg1, .result = result};
	add_iroperation(ir, op);
}
