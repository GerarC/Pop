#include "../include/constant/common.constants.h"
#include "../include/constant/main.constants.h"
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
	log_error(USAGE_MESSAGE, program, program, program);
	exit(EXIT_ERROR_CODE);
}

int main(int argc, char **argv) {

	if (argc < 3) print_usage(argv[0]);
	log_info(POP_COMPILER);

	int compile = 0;
	int out_file = 0;
	int debug = 0;
	int assembly = 0;
	char *source_file;
	char *output_file;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], OPTION_COMPILE) == 0) {
			if (compile == 1) goto cli_error;
			i += 1;
			source_file = argv[i];
			compile = 1;
			continue;
		} else if (strcmp(argv[i], OPTION_OUTPUT) == 0) {
			i += 1;
			output_file = argv[i];
			out_file = 1;
			continue;
		} else if (strcmp(argv[i], OPTION_DEBUG) == 0) {
			debug = 1;
		} else {
		cli_error:
			log_fatal(ERROR_BAD_ARGUMENT_MESSAGE, argv[i]);
			exit(1);
		}
	}

	if (compile) {
		const char *program = read_file(source_file);
		SymbolTable *table = create_table();

		Lexer lex = create_lexer(source_file);
		lex_program(&lex, program);
		if (debug) print_lexer(&lex);

		Parser parser = create_parser(&lex);
		Node *ast = parse_program(&parser);
		if (debug) print_ast(ast);

		semantic_analysis(ast, table);

		IntermediateRepresentation *ir =
			create_intermediate_representation(ast, table);
		if (debug) print_ir(ir->globals);
		if (debug) print_ir(ir);

		generate_nasm_x86_64(output_file, ir, table);

		free_intermediate_representation(ir);
		free_lexer(lex);
		free_ast(ast);
		free_symbol_table(table);
	}
	return 0;
}
