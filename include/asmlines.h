#ifndef ASMLINES_H
#define ASMLINES_H

#include "typedef.h"

typedef struct _assembly_lines_t {
	char **lines;
	size count;
	size capacity;
} Assembler;

Assembler create_assembler();

void add_line(Assembler* assembler, const char *asm_line);

void free_assembler(Assembler assembler);

#endif // !ASMLINES_H
