#include "../../include/generation/nasm_x86_64.h"
#include "../../include/config.h"
#include "../../include/log.h"
#include "../../include/typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *dest;
SymbolTable *nasm_table;
IntermediateRepresentation *ir;
void generation_error(const char *message, IrOperation *op);

// Registers stuff begin
const char *registers[4] = {"r8", "r9", "r10", "r11"};
char asm_line[MAX_ASM_LINE_SIZE];
int free_reg[4] = {1, 1, 1, 1};

void deallocate_all_registers() {
	for (int i = 0; i < 4; i++)
		free_reg[i] = 1;
}

static int allocate_register() {
	for (int i = 0; i < 4; i++)
		if (free_reg[i] == 1) {
			free_reg[i] = 0;
			return i;
		}
	generation_error("Not free registers", NULL);
	return -1;
}

void deallocate_register(size index) {
	if (free_reg[index] != 0)
		generation_error("Error deallocating register", NULL);
	free_reg[index] = 1;
}
// end registers stuff
char *val_string(IrValue value);

int load_value(IrValue value);
void move_reg(int r1, int r2);

/// Binary operations
int gen_add(int r1, int r2);
int gen_sub(int r1, int r2);
int gen_mul(int r1, int r2);
int gen_div(int r1, int r2);
int gen_comparison(int r1, int r2, IrOperationType type);
void generate_binaryop(int index);

/// Unitary operations
int gen_bin_not(int r1);
int gen_uminus(int r1);
void generate_unitaryop(int index);

/// Variables
void generate_glob_decl(int index);
void generate_assign(int index);

/// If and while statements
void generate_ifdo(int index);
void generate_else(int index);
void generate_endblock(int index);
void generate_while(int index);

void generate_temp_print_int(int index);
void generate_temp_print_char(int index);

void generate_operations();
void generate_header();
void generate_footer();

void generate_nasm_x86_64(const char *destination,
						  IntermediateRepresentation *inrepr,
						  SymbolTable *tbl) {
	dest = fopen(destination, "w");
	nasm_table = tbl;
	ir = inrepr;
	if (nasm_table == NULL) generation_error("Symbol table doesn't exists", NULL);
	if (ir == NULL) generation_error("IR doesn't exists", NULL);
	if (dest == NULL) generation_error("Cannot create destination file", NULL);

	deallocate_all_registers();
	generate_header();

	generate_operations();

	generate_footer();
	fclose(dest);
}

void generate_operations() {
	log_info("NASM code generation");
	size i;
	for (i = 0; i < ir->count; i++) {
		IrOperation op = ir->instructions[i];
		int arg1, arg2;
		switch (op.type) {
			case IR_DECLARATION:
				generate_glob_decl(i);
				break;
			case IR_ASSIGNMENT:
				generate_assign(i);
				break;

			case IR_IF:
			case IR_DO:
				generate_ifdo(i);
				break;
			case IR_ELSE:
				generate_else(i);
				break;

			case IR_WHILE:
				generate_while(i);
				break;

			case IR_EQUAL:
			case IR_DIFF:
			case IR_GT:
			case IR_GEQT:
			case IR_LT:
			case IR_LEQT:

			case IR_ADD:
			case IR_SUB:
			case IR_MUL:
			case IR_DIV:
				generate_binaryop(i);
				break;
			case IR_UMINUS:
			case IR_NOT:
			case IR_BINNOT:
				generate_unitaryop(i);
				break;

			case IR_ENDBLOCK:
				generate_endblock(i);
				break;

			case IR_TEMP_PRINT_INT:
				generate_temp_print_int(i);
				break;
			case IR_TEMP_PRINT_CHAR:
				generate_temp_print_char(i);
				break;

			default:
				generation_error("OP NOT IMPLEMENTED", &op);
		}
	}
}

void generate_header() {
	fprintf(dest, "BITS 64\n"
				  "\n;  ---------- Initializate program ----------\n"
				  "section .text\n"
				  "\tglobal _start\n"

				  // Temporal print
				  "print_int:\n"
				  "\tmov\tr9, -3689348814741910323\n"
				  "\tsub\trsp, 40\n"
				  "\tmov\tBYTE [rsp+31], 10\n"
				  "\tlea\trcx, [rsp+30]\n"
				  ".L2:\n"
				  "\tmov\trax, rdi\n"
				  "\tlea\tr8, [rsp+32]\n"
				  "\tmul\tr9\n"
				  "\tmov\trax, rdi\n"
				  "\tsub\tr8, rcx\n"
				  "\tshr\trdx, 3\n"
				  "\tlea\trsi, [rdx+rdx*4]\n"
				  "\tadd\trsi, rsi\n"
				  "\tsub\trax, rsi\n"
				  "\tadd\teax, 48\n"
				  "\tmov\tBYTE [rcx], al\n"
				  "\tmov\trax, rdi\n"
				  "\tmov\trdi, rdx\n"
				  "\tmov\trdx, rcx\n"
				  "\tsub\trcx, 1\n"
				  "\tcmp\trax, 9\n"
				  "\tja \t.L2\n"
				  "\tlea\trax, [rsp+32]\n"
				  "\tmov\tedi, 1\n"
				  "\tsub\trdx, rax\n"
				  "\txor\teax, eax\n"
				  "\tlea\trsi, [rsp+32+rdx]\n"
				  "\tmov\trdx, r8\n"
				  "\tmov\trax, 1\n"
				  "\tsyscall\n"
				  "\tadd     rsp, 40\n"
				  "\tret\n\n"

				  "print_char:\n"
				  "\tpush\trdi\n"
				  "\tmov\tedi, 1\n"
				  "\tmov\trax, 1\n"
				  "\tmov\trsi, rsp\n"
				  "\tmov\trdx, 1\n"
				  "\tsyscall\n"
				  "\tadd\trsp, 8\n"
				  "\tret\n\n"

				  // Program
				  "_start:\n"
				  "\tpush\trbp\n"
				  "\tmov\trbp, rsp\n"

	);
}

void generate_footer() {
	fprintf(dest, "\n;  ---------- Terminate program ----------\n"
				  "\tmov\teax, 1\n"
				  "\tmov\tebx, 0\n"
				  "\tint\t80h\n");
}

char *val_string(IrValue value) {
	char *string_val = (char *)malloc(sizeof(char) * MAX_SYMBOL_SIZE);
	switch (value.type) {
		case IRVAL_INT:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%i", value.data.ival);
			break;
		case IRVAL_FLOAT:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%.2f", value.data.fval);
			break;
		case IRVAL_ADDRESS:
			snprintf(string_val, MAX_SYMBOL_SIZE, ".POP%i", value.data.index);
			break;
		case IRVAL_IDENTIFIER:
			snprintf(string_val, MAX_SYMBOL_SIZE, "[%s]", value.data.ident);

			break;
		case IRVAL_CHAR:
			snprintf(string_val, MAX_SYMBOL_SIZE, "%X", value.data.ival);
			break;
	}
	return string_val;
}

// Single operation writters
int load_value(IrValue value) {
	char *arg = val_string(value);
	int r;

	if (value.type == IRVAL_INT || value.type == IRVAL_CHAR) {
		r = allocate_register();
		fprintf(dest, "\tmov\t%s, %s\n", registers[r], arg);

		free(arg);
		log_info("enters");
		return r;
	} else if (value.type == IRVAL_ADDRESS) {
		return ir->instructions[value.data.index].result.data.index;
	} else if (value.type == IRVAL_IDENTIFIER) {
		r = allocate_register();
		fprintf(dest, "\tmov\t%s, %s\n", registers[r], arg);

		free(arg);
		return r;
	}
	log_fatal("Value %i type NOT IMPLEMENTED", value.type);
	exit(1);
	return -1;
}

void move_reg(int r1, int r2) {
	fprintf(dest, "\tmov\t%s, %s\n", registers[r1], registers[r2]);
}

int gen_add(int r1, int r2) {
	fprintf(dest, "\tadd\t%s, %s\n", registers[r1], registers[r2]);
	deallocate_register(r2);
	return r1;
}

int gen_sub(int r1, int r2) {
	fprintf(dest, "\tsub\t%s, %s\n", registers[r1], registers[r2]);
	deallocate_register(r2);
	return r1;
}

int gen_mul(int r1, int r2) {
	fprintf(dest, "\timul\t%s, %s\n", registers[r1], registers[r2]);
	deallocate_register(r2);
	return r1;
}

int gen_div(int r1, int r2) {
	fprintf(dest,
			"\tmov\trax, %s\n"
			"\tcqo\n"
			"\tidiv\t%s\n"
			"\tmov\t%s, rax\n",
			registers[r1], registers[r2], registers[r1]);
	deallocate_register(r2);
	return r1;
}

int gen_comparison(int r1, int r2, IrOperationType type) {
	if (type != IR_DIFF)
		fprintf(dest,
				"\tmov\trcx, 0\n"
				"\tmov\trdx, 1\n"
				"\tcmp\t%s, %s\n",
				registers[r1], registers[r2]);
	else
		fprintf(dest,
				"\tmov\trcx, 1\n"
				"\tmov\trdx, 0\n"
				"\tcmp\t%s, %s\n",
				registers[r1], registers[r2]);

	switch (type) {
		case IR_EQUAL:
		case IR_DIFF:
			fprintf(dest, "\tcmove\trcx, rdx\n");
			break;
		case IR_GT:
			fprintf(dest, "\tcmovg\trcx, rdx\n");
			break;
		case IR_GEQT:
			fprintf(dest, "\tcmovge\trcx, rdx\n");
			break;
		case IR_LT:
			fprintf(dest, "\tcmovl\trcx, rdx\n");
			break;
		case IR_LEQT:
			fprintf(dest, "\tcmovle\trcx, rdx\n");
			break;

		default:
			log_fatal("%i COMPARISON NOT IMPLEMENTED", type);
			exit(1);
			break;
	}

	fprintf(dest, "\tmov\t%s, rcx\n", registers[r1]);
	deallocate_register(r2);
	return r1;
}

void generate_binaryop(int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(op.arg1);
	int arg2 = load_value(op.arg2);
	switch (op.type) {
		case IR_ADD:
			ir->instructions[index].result.data.index = gen_add(arg1, arg2);
			break;
		case IR_SUB:
			ir->instructions[index].result.data.index = gen_sub(arg1, arg2);
			break;
		case IR_MUL:
			ir->instructions[index].result.data.index = gen_mul(arg1, arg2);
			break;
		case IR_DIV:
			ir->instructions[index].result.data.index = gen_div(arg1, arg2);
			break;

		case IR_EQUAL:
		case IR_DIFF:
		case IR_GT:
		case IR_GEQT:
		case IR_LT:
		case IR_LEQT:
			ir->instructions[index].result.data.index =
				gen_comparison(arg1, arg2, op.type);
			break;

		default:
			generation_error("%i OP NOT IMPLEMENTED", &op);
	}
}

int gen_bin_not(int r1) {
	fprintf(dest, "\tNOT %s\n", registers[r1]);
	return r1;
}

int gen_uminus(int r1) {
	fprintf(dest,
			"\tNOT %s\n"
			"\tinc\t%s\n",
			registers[r1], registers[r1]);

	return r1;
}

void generate_unitaryop(int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(op.arg1);
	switch (op.type) {
		case IR_UMINUS:
			ir->instructions[index].result.data.index = gen_uminus(arg1);
			break;
		case IR_BINNOT:
			ir->instructions[index].result.data.index = gen_bin_not(arg1);
			break;

		default:
			generation_error("%i OP NOT IMPLEMENTED", &op);
	}
}

void generate_glob_decl(int index) {
	// TODO: Prepare things to do this with any type, not only int
	IrOperation op = ir->instructions[index];
	if (op.arg1.type == IRVAL_IDENTIFIER) {
		/*
		if (strcmp(type, "int")) {
			fprintf(dest, "\tcommon\t%s 8:8\n",
					 op.arg1.data.ident);

		} else if (strcmp(type, "char")) {
			fprintf(dest, "\tcommon\t%s 1:1\n",
					 op.arg1.data.ident);

		} else if (strcmp(type, "bool")) {
			fprintf(dest, "\tcommon\t%s 1:1\n",
					 op.arg1.data.ident);

		}
		return;
	*/
	}
	generation_error("%i OP TYPE NOT IMPLEMENTED", &op);
}

void generate_assign(int index) {
	/* FIX: multi assignation if broken. At the moment a = b = c isn't available
	 * */
	IrOperation op = ir->instructions[index];

	int r = load_value(op.arg2);
	fprintf(dest, "\tmov\t%s, %s\n", val_string(op.arg1), registers[r]);
	deallocate_register(r);
}

void generate_ifdo(int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(op.arg1);
	char *addr = val_string(op.result);
	fprintf(dest,
			"\ttest\t%s, %s\n"
			"\tjz\t%s\n",
			registers[arg1], registers[arg1], addr);
	free(addr);
	deallocate_register(arg1);
}

void generate_else(int index) {
	IrOperation op = ir->instructions[index];
	char *self_addr = val_string(op.arg1);
	char *addr = val_string(op.result);
	fprintf(dest,
			"\tjmp\t%s\n"
			"%s:\n",
			addr, self_addr);
	free(addr);
	free(self_addr);
}

void generate_endblock(int index) {
	IrOperation op = ir->instructions[index];
	char *addr = NULL;
	if (op.arg1.type == IRVAL_ADDRESS) {
		addr = val_string(op.arg1);
		fprintf(dest, "\tjmp\t%s\n", addr);
		free(addr);
	}
	addr = val_string(op.result);
	fprintf(dest, "%s:\n", addr);
	free(addr);
}

void generate_while(int index) {
	IrOperation op = ir->instructions[index];
	char *addr = val_string(op.result);

	fprintf(dest, "%s:\n", addr);
	free(addr);
}

void generate_temp_print_int(int index) {
	IrOperation op = ir->instructions[index];
	int r = load_value(op.arg1);
	fprintf(dest,
			"\tmov\trdi, %s\n"
			"\tcall\tprint_int\n",
			registers[r]);
	deallocate_register(r);
}

void generate_temp_print_char(int index) {
	IrOperation op = ir->instructions[index];
	int r = load_value(op.arg1);
	fprintf(dest,
			"\tmov\trdi, %s\n"
			"\tcall\tprint_char\n",
			registers[r]);
	deallocate_register(r);
}

void generation_error(const char *message, IrOperation *op) {
	if (op == NULL) log_fatal(message);
	else log_fatal("%s: %i", message, op->type);
	exit(1);
}
