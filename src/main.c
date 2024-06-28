#include "../include/file.h"
#include "../include/ir.h"
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

	int compile = 0;
	int assembly = 0;
	char *source_file;
	char *output_file;
	int lexer_len;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			if (compile == 1) goto cli_error;
			source_file = argv[i + 1];
			i += 2;
			compile = 1;
			continue;
		} else if (strcmp(argv[i], "-o") == 0) {
			output_file = argv[i + 1];
			i += 2;
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
		IntermediateRepresentation *ir = create_intermediate_representation(ast);
        print_ir(ir);


		free_lexer(tokens, lexer_len);
		log_info("free ast");
		free_ast(ast);
	}
	return 0;
}
