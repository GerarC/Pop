#include "../include/asmlines.h"
#include "../include/file.h"
#include "../include/generation/nasm_x86_64.h"
#include "../include/ir.h"
#include "../include/lexer.h"
#include "../include/log.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include <stdlib.h>
#include <string.h>

void print_usage(char *program) {
	log_error("cli error.\n"
			  "USAGE:\n"
			  "\t%s -c <file> -> To compile the program\n"
			  "\t%s -s <file> -> To create only assembler code\n"
			  "\t%s -o <file> -> output file\n",
			  program, program, program);
	exit(1);
}

int main(int argc, char **argv) {

	if (argc < 3) print_usage(argv[0]);
	log_info("Pop compiler");
    log_warn("%i", 't');

	int compile = 0;
	int out_file = 0;
	int debug = 0;
	int assembly = 0;
	char *source_file;
	char *output_file;

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
		} else if (strcmp(argv[i], "--debug") == 0) {
			debug = 1;
		} else {
		cli_error:
			log_fatal("Bad argument '%s'", argv[i]);
			exit(1);
		}
	}

	if (compile) {
		const char *program = read_file(source_file);

		Lexer lex = create_lexer(source_file);
		lex_program(&lex, program);
		if (debug) print_lexer(&lex);

		Parser parser = create_parser(&lex);
		Node *ast = parse_program(&parser);
		if (debug) print_ast(ast);

		semantic_analysis(ast);

		IntermediateRepresentation *ir =
			create_intermediate_representation(ast);
		if (debug) print_ir(ir);

		Assembler code = create_assembler();
		generate_nasm_x86_64(&code, ir);
		write_file(output_file, &code);
		free_assembler(code);

		free_intermediate_representation(ir);
		free_lexer(lex);
		free_ast(ast);
	}
	return 0;
}
