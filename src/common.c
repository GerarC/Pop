#include "../include/common.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *token_string(Token tok) {
	static char tkn_str[MAX_TOK_STRING_SIZE];
	char *token_type;
	switch (tok.type) {
		case TOK_INVALID:
			token_type = strdup("Invalid");
			break;

		case TOK_PLUS:
		case TOK_MINUS:
		case TOK_SLASH:
		case TOK_STAR:
		case TOK_MOD:
			token_type = strdup("Math Op 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_NUM:
			token_type = strdup("Integer");
			break;

		case TOK_LPAREN:
		case TOK_LCURLY:
		case TOK_LBRACE:
		case TOK_LANGLE:
			token_type = strdup("OpGroup 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_RPAREN:
		case TOK_RCURLY:
		case TOK_RBRACE:
		case TOK_RANGLE:
			token_type = strdup("ClGroup 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_ASSIGN:
		case TOK_PLUS_EQUAL:
		case TOK_MINUS_EQUAL:
		case TOK_STAR_EQUAL:
		case TOK_SLASH_EQUAL:
			token_type = strdup("Assign 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_PLUS_PLUS:
		case TOK_MINUS_MINUS:
			token_type = strdup("Move one 'xx'");
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_SYMBOL:
			token_type = strdup("Symbol");
			break;

		case TOK_BOOL:
			token_type = strdup("Boolean");
			break;

		case TOK_EQUAL:
		case TOK_DIFF:
		case TOK_GEQT:
		case TOK_LEQT:
			token_type = strdup("Comparison 'xx'");
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_AND:
		case TOK_OR:
			token_type = strdup("Logical 'xx'");
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_GT:
		case TOK_LT:
			token_type = strdup("Comparison 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_BINOR:
		case TOK_BINAND:
		case TOK_BINXOR:
			token_type = strdup("Binary 'x'");
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_IF:
			token_type = strdup("if");
			break;

		case TOK_ELSE:
			token_type = strdup("else");
			break;

		case TOK_NOT:
			token_type = strdup("LOGICAL_NOT");
			break;

		case TOK_ELN:
			token_type = strdup("line's end");
			break;

		case TOK_EOF:
			token_type = strdup("file's end");
			break;

		default:
			log_warn("\tToken %i THIS SHOULD BE UNREACHEABLE", tok.type);
			token_type = strdup("NOT IMPLEMENTED");
			break;
	}

	if (tok.type == TOK_NUM) {
		snprintf(tkn_str, MAX_TOK_STRING_SIZE,
				 "Token{ type: %s, location: (%i, %i), value: %d }", token_type,
				 tok.location.line, tok.location.col, *(int *)tok.value);
	} else {
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), lexeme: %s }",
				 token_type, tok.location.line, tok.location.col, tok.lexeme);
	}

	free(token_type);
	return tkn_str;
}

void create_token(Token *token, TokenType type, TokenLocation loc, int length,
				  char *text, void *value) {
	token->type = type;
	token->location = loc;
	token->length = length;
	token->lexeme = text;
	token->value = value;
}

void free_token(Token *tok) {
	if (tok == NULL) {
		log_warn("Token to free should exist");
		return;
	}
	if (tok->lexeme != NULL) free(tok->lexeme);
	if (tok->value != NULL) free(tok->value);
	/*free(tok);*/;
}
