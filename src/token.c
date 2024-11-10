#include "../include/token.h"
#include "../include/constant/token_constants.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *token_string(Token tok) {
	static char tkn_str[MAX_TOK_STRING_SIZE];
	char *token_type;
	switch (tok.type) {
		case TOK_INVALID:
			token_type = strdup(TOKEN_TYPE_TEXT_INVALID);
			break;

		case TOK_PLUS:
		case TOK_MINUS:
		case TOK_SLASH:
		case TOK_STAR:
		case TOK_MOD:
			token_type = strdup(TOKEN_TYPE_TEXT_MATH_OP);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_INT:
			token_type = strdup(TOKEN_TYPE_TEXT_INTEGER);
			break;

		case TOK_FLOAT:
			token_type = strdup(TOKEN_TYPE_TEXT_FLOAT);
			break;

		case TOK_LPAREN:
		case TOK_LCURLY:
		case TOK_LBRACE:
		case TOK_LANGLE:
			token_type = strdup(TOKEN_TYPE_TEXT_OPEN_GROUP);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_RPAREN:
		case TOK_RCURLY:
		case TOK_RBRACE:
		case TOK_RANGLE:
			token_type = strdup(TOKEN_TYPE_TEXT_CLOSE_GROUP);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_ASSIGN:
		case TOK_PLUS_EQUAL:
		case TOK_MINUS_EQUAL:
		case TOK_STAR_EQUAL:
		case TOK_SLASH_EQUAL:
			token_type = strdup(TOKEN_TYPE_TEXT_ASSIGN);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_PLUS_PLUS:
		case TOK_MINUS_MINUS:
			token_type = strdup(TOKEN_TYPE_TEXT_UNITARY_OPERATION);
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_IDENTIFIER:
			token_type = strdup(TOKEN_TYPE_TEXT_IDENTIFIER);
			break;

		case TOK_INTTYPE:
		case TOK_STRTYPE:
		case TOK_LONGTYPE:
		case TOK_CHARTYPE:
		case TOK_BOOLTYPE:
			token_type = strdup(TOKEN_TYPE_TEXT_PRIMITIVE);
			break;

		case TOK_BOOL:
			token_type = strdup(TOKEN_TYPE_TEXT_BOOLEAN);
			break;

		case TOK_CHAR:
			token_type = strdup(TOKEN_TYPE_TEXT_CHAR);
			break;

		case TOK_COMMA:
			token_type = strdup(TOKEN_TYPE_TEXT_COMMA);
			break;

		case TOK_EQUAL:
		case TOK_DIFF:
		case TOK_GEQT:
		case TOK_LEQT:
			token_type = strdup(TOKEN_TYPE_TEXT_COMPARATOR);
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_AND:
		case TOK_OR:
			token_type = strdup(TOKEN_TYPE_TEXT_LOGICAL);
			token_type[strlen(token_type) - 3] = ((char *)tok.lexeme)[0];
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[1];
			break;

		case TOK_GT:
		case TOK_LT:
			token_type = strdup(TOKEN_TYPE_TEXT_SINGLE_COMPARATOR);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_BINOR:
		case TOK_BINAND:
		case TOK_BINXOR:
			token_type = strdup(TOKEN_TYPE_TEXT_BINARY_OPERATOR);
			token_type[strlen(token_type) - 2] = ((char *)tok.lexeme)[0];
			break;

		case TOK_IF:
		case TOK_ELSE:
		case TOK_WHILE:
			token_type = strdup(tok.lexeme);
			break;

		case TOK_NOT:
			token_type = strdup(TOKEN_TYPE_TEXT_LOGICAL_NOT);
			break;

		case TOK_ELN:
			token_type = strdup(TOKEN_TYPE_TEXT_ELN);
			break;

		case TOK_EOF:
			token_type = strdup(TOKEN_TYPE_TEXT_EOF);
			break;

		case TOK_RETURN:
			token_type = strdup(TOKEN_TYPE_TEXT_RETURN);
			break;

		case TOK_PRINT_INT: // TEMPORAL
		case TOK_PRINT_CHAR:
			token_type = strdup(TOKEN_TYPE_TEXT_PRINT);
			break;

		default:
			token_type = strdup(TOKEN_TYPE_TEXT_NOT_IMPLEMENTED);
			break;
	}

	snprintf(tkn_str, sizeof(tkn_str), TOKEN_STRING_TOK_TEMPLATE, token_type,
			 tok.location.line, tok.location.col, tok.lexeme);

	free(token_type);
	return tkn_str;
}

void create_token(Token *token, TokenType type, SourceLocation loc, int length,
				  char *text, void *value) {
	token->type = type;
	token->location = loc;
	token->length = length;
	token->lexeme = text;
	token->value = value;
}

void free_token(Token *tok) {
	if (tok == NULL) {
		log_warn(TOKEN_DEBUG_NOT_TOKEN_TO_CLEAN_MESSAGE);
		return;
	}
	if (tok->lexeme != NULL) free(tok->lexeme);
	if (tok->value != NULL) free(tok->value);
}
