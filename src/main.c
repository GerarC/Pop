#include "../include/file.h"
#include "../include/lexer.h"
#include "../include/log.h"
#include <stdio.h>
#include <string.h>

void print_usage() {
	log_error("You must pass a file to compile");
	printf("USAGE:\n"
		   "\t./pop -c <file> -> To compile the program\n"
		   "\t./pop -s <file> -> To create only assembler code\n");
}

int main(int argc, char **argv) {
	if (argc < 3) {
		print_usage();
		return 0;
	}

	char **program;
	int line_counter;
	log_debug("%s %s", argv[1], argv[2]);
	program = initialize_lines();
	read_file(argv[2], program, &line_counter);
	log_debug("Code lines: %i", line_counter);
	lex_program(argv[2], (const char **)program, line_counter);
	if (strcmp(argv[1], "-c") == 0) {
	}

	free_lines(program);
	return 0;
}
