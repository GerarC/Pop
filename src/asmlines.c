#include "../include/asmlines.h"
#include <stdlib.h>
#include <string.h>

Assembler create_assembler() {
	Assembler assembler = {
		.lines = (char **)malloc(sizeof(char *)), .count = 0, .capacity = 1};
	return assembler;
}

void free_assembler(Assembler assembler) {
	for (size i = 0; i < assembler.count; i++)
		free(assembler.lines[i]);
	free(assembler.lines);
}

void add_line(Assembler *assembler, const char *asm_line) {
	if (assembler->count >= assembler->capacity) {
		assembler->capacity *= 2;
		assembler->lines =
			realloc(assembler->lines, sizeof(char *) * assembler->capacity);
	}
	assembler->lines[assembler->count] = strdup(asm_line);
	assembler->count++;
}
