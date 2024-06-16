#include "../include/parser.h"
#include "../include/log.h"

// I'm doing this to have something graphic and it will be updating
// Parenthesis without quotes are for grouping
/* ### Grammar
 * Program -> statement*
 * statement -> expr tok_eln
 *            | assign tok_eln
 *            | declaration tok_eln
 * expr -> factor
 *       | term
 *       | comparison
 *       | literal
 * comparison -> term (('<'|'>'|'<='|'>='|'=='|'!=') comparison)*
 * term -> factor (('+'|'-') term)*
 *       | '(' term ')'
 * factor -> literal (('*'|'/'|'%') factor)*
 *         | '(' factor ')'
 * literal -> number | term
 * */

/*
 * */
Parser create_parser(Token *tokens, int length) {
	Parser parser = {tokens, length, 0};
	return parser;
}

Token current_token(Parser *parser) { return parser->tokens[parser->pos]; }

void next(Parser *parser) {
	if (parser->pos < parser->length) parser->pos++;
}

Node *parse_program(Parser *parser) {
	log_info("Parsing Tokens");
	Node *statements = NULL;
	Node **current = &statements;

    return statements;
 
}
