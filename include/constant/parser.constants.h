#ifndef PARSER_CONSTANTS_H
#define PARSER_CONSTANTS_H

#define PARSER_DEFAULT_MAIN_LEXEME "main ."
#define PARSER_DEFAULT_GLUE_LEXEME "glue"

// Debug
#define PARSER_DEBUG_PROCESS_STARTS "Syntactic analysis"
#define PARSER_DEBUG_TOKEN_TEMPLATE "token stmt %s"
#define PARSER_DEBUG_PRINT_AST "Print AST"

#define PARSER_PRINT_MAX_PREFIX_SIZE 1024
#define PARSER_PRINT_PREFIX_TEMPLATE "%s"
#define PARSER_PRINT_CONNECTION_LEFT "├─ "
#define PARSER_PRINT_CONNECTION_RIGHT "└─ "
#define PARSER_PRINT_PREFIX_LEFT "│  "
#define PARSER_PRINT_PREFIX_RIGHT "   "
#define PARSER_PRINT_TOKEN_MAIN ".\n"
#define PARSER_PRINT_TOKEN_BLOCK_GLUE "{}\n"
#define PARSER_PRINT_TOKEN_NULL "null\n"
#define PARSER_PRINT_TOKEN_TEMPLATE "%s\n"

// Errors
#define PARSER_ERROR_MESSAGE_TEMPLATE "%s:%i:%i '%s': %s"
#define PARSER_ERROR_NOT_PARENT_NODE_TO_ADD_MESSAGE                            \
	"A parent must not be null to add a children"
#define PARSER_ERROR_NOT_CHILD_PROVIDED_MESSAGE                                \
	"You cannot add NULL as child to %s"
#define PARSER_ERROR_NOT_PARSED_TOKENS_AT_PROGRAM_MESSAGE                      \
	"Error parsing a statement at program"
#define PARSER_ERROR_AN_IDENTIFIER_WAS_EXPECTED_MESSAGE                        \
	"An identifier was expected after a type"
#define PARSER_ERROR_WRONG_IDENTIFIER_FORMAT_MESSAGE                           \
	"Error parsing declaration of %s"
#define PARSER_ERROR_COMMA_EXPECTED_MESSAGE "There must be a comma"
#define PARSER_ERROR_CLOSING_PARENTHESIS_EXPECTED_MESSAGE "A ')' was expected"
#define PARSER_ERROR_CLOSING_CURLY_BRACE_EXPECTED_MESSAGE "A '}' was expected"
#define PARSER_ERROR_WRONG_ASSIGNMENT_PARSING_MESSAGE                          \
	"There's no value to assign"
#define PARSER_ERROR_WRONG_COMPARISON_STATEMENT_PARSING_MESSAGE                \
	"There's is an invalid comparison format"
#define PARSER_ERROR_WRONG_TERM_STATEMENT_PARSING_MESSAGE                      \
	"There's is an invalid term format"
#define PARSER_ERROR_WRONG_FACTOR_STATEMENT_PARSING_MESSAGE                    \
	"There's is an invalid factor format"
#define PARSER_ERROR_WRONG_UNITARY_STATEMENT_PARSING_MESSAGE                   \
	"There's is an invalid unitary format"
#define PARSER_ERROR_UNEXPECTED_LITERAL_TOKEN "Unexpected literal token"
#define PARSER_ERROR_WRONG_STATEMENT_FORMAT "Error in format of statement"

#endif // !PARSER_CONSTANTS_H
