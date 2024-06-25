#include "../include/file.h"
#include "../include/lexer.h"
#include "../include/log.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include <stdlib.h>
#include <string.h>

void print_usage() {
	log_error("cli error.\n"
        "USAGE:\n"
		   "\t./pop -c <file> -> To compile the program\n"
		   "\t./pop -s <file> -> To create only assembler code\n"
		   "\t./pop -o <file> -> output file\n");
}

int main(int argc, char **argv) {
	if (argc < 3) {
		print_usage();
		return -1;
	}
	log_info("Pop compiler");

	char **program;
	int compile = 0;
	int assembly = 0;
	char *source_file;
	char *final_file;
	int program_len, lexer_len;
	program = initialize_lines();

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			if (compile == 1) goto cli_error;
			source_file = argv[i + 1];
			i += 2;
			compile = 1;
			continue;
		} else if (strcmp(argv[i], "-c") == 0) {
			final_file = argv[i + 1];
			i += 2;
			continue;
		} else {
		cli_error:
			log_fatal("Bad argument '%s'", argv[i]);
			exit(1);
		}
	}

	if (compile) {
		if (read_file(source_file, program, &program_len) == -1) return -1;

		Token *tokens = lex_program(source_file, (const char **)program,
									program_len, &lexer_len);

		Parser parser = create_parser(tokens, lexer_len);
		Node *ast = parse_program(&parser);

		log_info("Print AST");
		print_ast(ast, "x", 0, 1);

        semantic_analysis(ast);

		free_lexer(tokens, lexer_len);
		free_lines(program);
		log_info("free ast");
		free_ast(ast);
	}
	return 0;
}
