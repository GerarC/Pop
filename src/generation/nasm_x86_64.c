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
		log_fatal("Error freeing register");
		exit(1);
	}
	free_reg[index] = 1;
}
// end registers stuff

void generate_operations(Assembler *code, IntermediateRepresentation *ir);
void generate_header(Assembler *code);
void generate_footer(Assembler *code);

void generate_nasm_x86_64(Assembler *code, IntermediateRepresentation *ir) {
	deallocate_all_registers();
	generate_header(code);

	generate_operations(code, ir);

	generate_footer(code);
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
	char *string_val = (char *)malloc(sizeof(char) * 10);
	switch (value.type) {
		case IRVAL_INT:
			snprintf(string_val, 10, "%i", value.data.ival);
			break;
		case IRVAL_FLOAT:
			snprintf(string_val, 10, "%.2f", value.data.fval);
			break;
		case IRVAL_ADDRESS:
			/*snprintf(string_val, 10, "%s", registers[allocate_register()]);*/
			break;
	}
	return string_val;
}

// Single operation writters
void temp_printint(Assembler *code, int r) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tmov\trdi, %s\n"
			 "\tcall\tprint\n",
			 registers[r]);
	add_line(code, asm_line);
}

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
	}
	log_fatal("Value type NOT IMPLEMENTED");
	exit(1);
}

void move_reg(Assembler *code, int r1, int r2) {
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
}

int gen_add(Assembler *code, int r1, int r2) {
	add_line(code, "\t;  ---- ADD ----\n");
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tadd\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_sub(Assembler *code, int r1, int r2) {
	add_line(code, "\t;  ---- SUB ----\n");
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tsub\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_mul(Assembler *code, int r1, int r2) {
	add_line(code, "\t;  ---- MUL ----\n");
	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\timul\t%s, %s\n", registers[r1],
			 registers[r2]);
	add_line(code, asm_line);
	deallocate_register(r2);
	return r1;
}

int gen_div(Assembler *code, int r1, int r2) {
	add_line(code, "\t;  ---- DIV ----\n");

	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\trax, %s\n", registers[r1]);
	add_line(code, asm_line);

	snprintf(asm_line, MAX_ASM_LINE_SIZE,
			 "\tcqo\n"
			 "\tidiv\t%s\n",
			 registers[r2]);
	add_line(code, asm_line);

	snprintf(asm_line, MAX_ASM_LINE_SIZE, "\tmov\t%s, rax\n", registers[r1]);
	add_line(code, asm_line);

	deallocate_register(r2);
	return r1;
}

void generate_operations(Assembler *code, IntermediateRepresentation *ir) {
	log_warn("NASM CODE GENERATION NOT IMPLEMENTED");
	size i;
	for (i = 0; i < ir->count; i++) {
		IrOperation op = ir->instructions[i];
		char asm_line[MAX_ASM_LINE_SIZE];
		int arg1, arg2;
		switch (op.type) {
			case IR_ADD:
				arg1 = load_value(code, ir, op.arg1);
				arg2 = load_value(code, ir, op.arg2);
				ir->instructions[i].result.data.index =
					gen_add(code, arg1, arg2);
				break;
			case IR_SUB:
				arg1 = load_value(code, ir, op.arg1);
				arg2 = load_value(code, ir, op.arg2);
				ir->instructions[i].result.data.index =
					gen_sub(code, arg1, arg2);
				break;
			case IR_MUL:
				arg1 = load_value(code, ir, op.arg1);
				arg2 = load_value(code, ir, op.arg2);
				ir->instructions[i].result.data.index =
					gen_mul(code, arg1, arg2);
				break;
			case IR_DIV:
				arg1 = load_value(code, ir, op.arg1);
				arg2 = load_value(code, ir, op.arg2);
				ir->instructions[i].result.data.index =
					gen_div(code, arg1, arg2);
				break;

			default:
				log_fatal("%i OP NOT IMPLEMENTED", op.type);
				exit(1);
				break;
		}
	}
	temp_printint(code, ir->instructions[i - 1].result.data.index);
}
