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
		return -1;
	}
	log_info("Pop compiler");

	char **program;
	int line_counter;
	program = initialize_lines();

	if (read_file(argv[2], program, &line_counter) == -1) return -1;

	lex_program(argv[2], (const char **)program, line_counter);

	if (strcmp(argv[1], "-c") == 0) {}

	free_lines(program);
	return 0;
}
