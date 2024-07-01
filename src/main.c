#include "../include/asmlines.h"
#include "../include/file.h"
#include "../include/generation/nasm_x86_64.h"
#include "../include/ir.h"
#include "../include/lexer.h"
#include "../include/log.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(char *program) {
	log_error("cli error.\n"
			  "USAGE:\n"
			  "\t%s -c <file> -> To compile the program\n"
			  "\t%s -s <file> -> To create only assembler code\n"
			  "\t%s -o <file> -> output file\n",
			  program, program, program);
}

int main(int argc, char **argv) {
	if (argc < 3) {
		print_usage(argv[0]);
		return -1;
	}
	log_info("Pop compiler");

	int compile = 0;
	int out_file = 0;
	int assembly = 0;
	char *source_file;
	char *output_file;
	int lexer_len;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			if (compile == 1) goto cli_error;
			i += 1;
			source_file = argv[i];
			compile = 1;
			continue;
		} else if (strcmp(argv[i], "-o") == 0) {
			i += 1;
			output_file = argv[i];
			out_file = 1;
			continue;
		} else {
		cli_error:
			log_fatal("Bad argument '%s'", argv[i]);
			exit(1);
		}
	}

	if (compile) {
		const char *program = read_file(source_file);
		log_trace("%s\n%s", source_file, program);

		Token *tokens = lex_program(source_file, program, &lexer_len);

		Parser parser = create_parser(tokens, lexer_len);
		Node *ast = parse_program(&parser);

		log_info("Print AST");
		print_ast(ast, "", 0, 1);
		semantic_analysis(ast);
		IntermediateRepresentation *ir =
			create_intermediate_representation(ast);
		print_ir(ir);

		Assembler code = create_assembler();

		generate_nasm_x86_64(&code, ir);
		write_file(output_file, &code);

		free_lexer(tokens, lexer_len);
		log_info("free ast");
		free_ast(ast);
	}
	return 0;
}
