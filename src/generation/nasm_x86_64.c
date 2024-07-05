#include "../../include/generation/nasm_x86_64.h"
#include "../../include/config.h"
#include "../../include/log.h"
#include "../../include/typedef.h"
#include <stdio.h>
#include <stdlib.h>

// Registers stuff begin
const char *registers[4] = {"r8", "r9", "r10", "r11"};
char asm_line[MAX_ASM_LINE_SIZE];
int free_reg[4] = {1, 1, 1, 1};

void deallocate_all_registers() {
	for (int i = 0; i < 4; i++)
		free_reg[i] = 1;
}

static int allocate_register() {
	for (int i = 0; i < 4; i++) {
		if (free_reg[i] == 1) {
			free_reg[i] = 0;
			return i;
		}
	}
	log_fatal("Not free registers");
	exit(1);
}

void deallocate_register(size index) {
	if (free_reg[index] != 0) {
		log_fatal("Error deallocating register");
		exit(1);
	}
	free_reg[index] = 1;
}
// end registers stuff
char *val_string(IrValue value);

int load_value(Assembler *code, IntermediateRepresentation *ir, IrValue value);
void move_reg(Assembler *code, int r1, int r2);

/// Binary operations
int gen_add(Assembler *code, int r1, int r2);
int gen_sub(Assembler *code, int r1, int r2);
int gen_mul(Assembler *code, int r1, int r2);
int gen_div(Assembler *code, int r1, int r2);
int gen_comparison(Assembler *code, int r1, int r2, IrOperationType type);
void generate_binaryop(Assembler *code, IntermediateRepresentation *ir,
					   int index);

/// Unitary operations
int gen_bin_not(Assembler *code, int r1);
int gen_uminus(Assembler *code, int r1);
void generate_unitaryop(Assembler *code, IntermediateRepresentation *ir,
						int index);

/// Variables
void generate_glob_decl(Assembler *code, IntermediateRepresentation *ir,
						int index);
void generate_assign(Assembler *code, IntermediateRepresentation *ir,
					 int index);

/// If and while statements
void generate_ifdo(Assembler *code, IntermediateRepresentation *ir, int index);
void generate_else(Assembler *code, IntermediateRepresentation *ir, int index);
void generate_endblock(Assembler *code, IntermediateRepresentation *ir,
					   int index);
void generate_while(Assembler *code, IntermediateRepresentation *ir, int index);

void generate_temp_print_int(Assembler *code, IntermediateRepresentation *ir,
							 int index);

void generate_operations(Assembler *code, IntermediateRepresentation *ir);
void generate_header(Assembler *code);
void generate_footer(Assembler *code);

void generate_nasm_x86_64(Assembler *code, IntermediateRepresentation *ir) {
	deallocate_all_registers();
	generate_header(code);

	generate_operations(code, ir);

	generate_footer(code);
}

void generate_operations(Assembler *code, IntermediateRepresentation *ir) {
	log_info("NASM code generation");
	size i;
	for (i = 0; i < ir->count; i++) {
		IrOperation op = ir->instructions[i];
		int arg1, arg2;
		switch (op.type) {
			case IR_DECLARATION:
				generate_glob_decl(code, ir, i);
				break;
			case IR_ASSIGNMENT:
				generate_assign(code, ir, i);
				break;

			case IR_IF:
			case IR_DO:
				generate_ifdo(code, ir, i);
				break;
			case IR_ELSE:
				generate_else(code, ir, i);
				break;

			case IR_WHILE:
				generate_while(code, ir, i);
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
				generate_binaryop(code, ir, i);
				break;
			case IR_UMINUS:
			case IR_NOT:
			case IR_BINNOT:
				generate_unitaryop(code, ir, i);
				break;

			case IR_ENDBLOCK:
				generate_endblock(code, ir, i);
				break;

			case IR_TEMP_PRINT_INT:
				generate_temp_print_int(code, ir, i);
				break;
		}
	}
}

void generate_header(Assembler *code) {
	add_line(code, "BITS 64\n"
				   "\n;  ---------- Initializate program ----------\n"
				   "section .text\n"
				   "\tglobal _start\n"

				   // Temporal print
				   "print:\n"
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

				   // Program
				   "_start:\n"
				   "\tpush\trbp\n"
				   "\tmov\trbp, rsp\n"

	);
}

void generate_footer(Assembler *code) {
	add_line(code, "\n;  ---------- Terminate program ----------\n"
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
	}
	return string_val;
}

// Single operation writters
int load_value(Assembler *code, IntermediateRepresentation *ir, IrValue value) {
	char *arg = val_string(value);
	int r;

	if (value.type == IRVAL_INT) {
		r = allocate_register();
		snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, %s\n", registers[r],
				 arg);
		add_line(code, asm_line);
		free(arg);
		return r;
	} else if (value.type == IRVAL_ADDRESS) {
		return ir->instructions[value.data.index].result.data.index;
	} else if (value.type == IRVAL_IDENTIFIER) {
		r = allocate_register();
		snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, %s\n", registers[r],
				 arg);
		add_line(code, asm_line);
		free(arg);
		return r;
	}
	log_fatal("Value %i type NOT IMPLEMENTED", value.type);
	exit(1);
}

void move_reg(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
}

int gen_add(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tadd\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_sub(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tsub\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_mul(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\timul\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_div(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tmov\trax, %s\n"
			 "\tcqo\n"
			 "\tidiv\t%s\n"
			 "\tmov\t%s, rax\n",
			 registers[r1], registers[r2], registers[r1]);
	add_line(code, asm_line);

	deallocate_register(r2);
	return r1;
}

int gen_comparison(Assembler *code, int r1, int r2, IrOperationType type) {
	if (type != IR_DIFF)
		snprintf(asm_line, MAX_ASM_LINE_SIZE,
				 "\tmov\trcx, 0\n"
				 "\tmov\trdx, 1\n"
				 "\tcmp\t%s, %s\n",
				 registers[r1], registers[r2]);
	else
		snprintf(asm_line, MAX_ASM_LINE_SIZE,
				 "\tmov\trcx, 1\n"
				 "\tmov\trdx, 0\n"
				 "\tcmp\t%s, %s\n",
				 registers[r1], registers[r2]);
	add_line(code, asm_line);

	switch (type) {
		case IR_EQUAL:
		case IR_DIFF:
			snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcmove\trcx, rdx\n");
			break;
		case IR_GT:
			snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcmovg\trcx, rdx\n");
			break;
		case IR_GEQT:
			snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcmovge\trcx, rdx\n");
			break;
		case IR_LT:
			snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcmovl\trcx, rdx\n");
			break;
		case IR_LEQT:
			snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcmovle\trcx, rdx\n");
			break;

		default:
			log_fatal("%i COMPARISON NOT IMPLEMENTED", type);
			exit(1);
			break;
	}
	add_line(code, asm_line);

	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, rcx\n", registers[r1]);
	add_line(code, asm_line);

	deallocate_register(r2);
	return r1;
}

void generate_binaryop(Assembler *code, IntermediateRepresentation *ir,
					   int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(code, ir, op.arg1);
	int arg2 = load_value(code, ir, op.arg2);
	switch (op.type) {
		case IR_ADD:
			ir->instructions[index].result.data.index =
				gen_add(code, arg1, arg2);
			break;
		case IR_SUB:
			ir->instructions[index].result.data.index =
				gen_sub(code, arg1, arg2);
			break;
		case IR_MUL:
			ir->instructions[index].result.data.index =
				gen_mul(code, arg1, arg2);
			break;
		case IR_DIV:
			ir->instructions[index].result.data.index =
				gen_div(code, arg1, arg2);
			break;

		case IR_EQUAL:
		case IR_DIFF:
		case IR_GT:
		case IR_GEQT:
		case IR_LT:
		case IR_LEQT:
			ir->instructions[index].result.data.index =
				gen_comparison(code, arg1, arg2, op.type);
			break;

		default:
			log_fatal("%i OP NOT IMPLEMENTED", op.type);
			exit(1);
			break;
	}
}

int gen_bin_not(Assembler *code, int r1) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tNOT %s\n", registers[r1]);
	add_line(code, asm_line);
	return r1;
}

int gen_uminus(Assembler *code, int r1) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tNOT %s\n"
			 "\tinc\t%s\n",
			 registers[r1], registers[r1]);
	add_line(code, asm_line);

	return r1;
}

void generate_unitaryop(Assembler *code, IntermediateRepresentation *ir,
						int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(code, ir, op.arg1);
	switch (op.type) {
		case IR_UMINUS:
			ir->instructions[index].result.data.index = gen_uminus(code, arg1);
			break;
		case IR_BINNOT:
			ir->instructions[index].result.data.index = gen_bin_not(code, arg1);
			break;

		default:
			log_fatal("%i OP NOT IMPLEMENTED", op.type);
			exit(1);
			break;
	}
}

void generate_glob_decl(Assembler *code, IntermediateRepresentation *ir,
						int index) {
	// TODO: Prepare things to do this with any type, not only int
	IrOperation op = ir->instructions[index];
	if (op.arg1.type == IRVAL_IDENTIFIER) {
		snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tcommon\t%s 8:8\n",
				 op.arg1.data.ident);
		add_line(code, asm_line);
		return;
	}

	log_fatal("%i type NOT IMPLEMENTED", op.arg1.type);
	exit(1);
}

void generate_assign(Assembler *code, IntermediateRepresentation *ir,
					 int index) {
	/* FIX: multi assignation if broken, fix that future Gerard. By the moment a
	 * = b = c isn't available
	 * */
	IrOperation op = ir->instructions[index];

	int r = load_value(code, ir, op.arg2);
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, %s\n",
			 val_string(op.arg1), registers[r]);
	deallocate_register(r);
	add_line(code, asm_line);
}

void generate_ifdo(Assembler *code, IntermediateRepresentation *ir, int index) {
	IrOperation op = ir->instructions[index];
	int arg1 = load_value(code, ir, op.arg1);
	char *addr = val_string(op.result);
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\ttest\t%s, %s\n"
			 "\tjz\t%s\n",
			 registers[arg1], registers[arg1], addr);
	free(addr);
	deallocate_register(arg1);
	add_line(code, asm_line);
}

void generate_else(Assembler *code, IntermediateRepresentation *ir, int index) {
	IrOperation op = ir->instructions[index];
	char *self_addr = val_string(op.arg1);
	char *addr = val_string(op.result);
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tjmp\t%s\n"
			 "%s:\n",
			 addr, self_addr);
	free(addr);
	free(self_addr);
	add_line(code, asm_line);
}

void generate_endblock(Assembler *code, IntermediateRepresentation *ir,
					   int index) {
	IrOperation op = ir->instructions[index];
	char *addr = NULL;
	if (op.arg1.type == IRVAL_ADDRESS) {
		addr = val_string(op.arg1);
		snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tjmp\t%s\n", addr);
		free(addr);
		add_line(code, asm_line);
	}
	addr = val_string(op.result);
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "%s:\n", addr);
	free(addr);
	add_line(code, asm_line);
}

void generate_while(Assembler *code, IntermediateRepresentation *ir,
					int index) {
	IrOperation op = ir->instructions[index];
	char *addr = val_string(op.result);

	snprintf(asm_line, MAX_ASM_LINE_SIZE, "%s:\n", addr);
	free(addr);
	add_line(code, asm_line);
}

void generate_temp_print_int(Assembler *code, IntermediateRepresentation *ir,
							 int index) {
	IrOperation op = ir->instructions[index];
	int r = load_value(code, ir, op.arg1);
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tmov\trdi, %s\n"
			 "\tcall\tprint\n",
			 registers[r]);
	add_line(code, asm_line);
	deallocate_register(r);
}
