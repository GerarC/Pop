#include "../include/file.h"
#include "../include/lexer.h"
#include "../include/log.h"
#include "../include/parser.h"
#include <stdio.h>

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
	int program_len, lexer_len;
	program = initialize_lines();

	if (read_file(argv[2], program, &program_len) == -1) return -1;

	Token *tokens =
		lex_program(argv[2], (const char **)program, program_len, &lexer_len);

	Parser parser = create_parser(tokens, lexer_len);
	Node *ast = parse_program(&parser);

log_info("Print AST");
    print_ast(ast,"x", 0, 1);

	free_lexer(tokens, lexer_len);
	free_lines(program);
	return 0;
}
