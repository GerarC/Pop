#include "../include/lexer.h"
#include "../include/log.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lex_program(const char *source, const char **program, int length) {
	Token *tokens;
	int token_line_count;
	log_info("Lexing initialized");
	for (int i = 0; i < length; i++) {
		token_line_count = 0;
		Token *line_tokens = lex_line(source, i, program[i], &token_line_count);
		for (int j = 0; j < token_line_count; j++) {
			log_debug("%s", token_string(line_tokens[j]));
		}
	}
	log_info("Lexing finished");
	return;
}

Token *lex_line(const char *source, int row, const char *line,
				int *token_count) {
	log_debug("Lexing line");
	Token *tok;
	Token *tokens = NULL;
	const char *start = NULL;
	const char *curr = line;
	int col = 0;
	TokenLocation loc;
	loc.line = row;
	strlcpy(loc.file, source, MAX_NAME_SIZE);
	*token_count = 0;
	while (*curr != '\0') {
		if (*token_count == 0) {
			tokens = (Token *)malloc(sizeof(Token));
			(*token_count)++;
		} else {
			(*token_count)++;
			tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));
		}
		if (isdigit(*curr)) {
			start = curr;
			loc.col = col;
			while (isdigit(*curr)) {
				curr++;
				col++;
			}
			int val_len = curr - start;
			char val_char[val_len + 1];
			strncpy(val_char, start, val_len);
			val_char[val_len] = '\0';
			int *val = (int *)malloc(sizeof(int));
			*val = atoi(val_char);

			create_token(&(tokens[*token_count - 1]), TOKEN_NUMBER, loc,
						 curr - start, NULL, val);
		} else if (isspace(*curr)) {
			while (isspace(*curr)) {
				curr++;
				col++;
			}
			(*token_count)--;
		} else if (*curr == '+') {
			start = curr;
			loc.col = col;
			curr++;
			col++;
			char *val = (char *)malloc(sizeof(char) * 2);
			strncpy(val, start, 1);
			val[1] = '\0';
			create_token(&(tokens[*token_count - 1]), TOKEN_PLUS, loc,
						 curr - start, NULL, val);
		} else {
			start = curr;
			loc.col = col;
			curr++;
			col++;
			char *val = (char *)malloc(sizeof(char) * 2);
			strncpy(val, start, 1);
			val[1] = '\0';
			create_token(&(tokens[*token_count - 1]), TOKEN_INVALID, loc,
						 curr - start, NULL, val);
		}
	}

	log_debug("Return tokens");
	return tokens;
}

static char *token_string(Token token) {
	static char tkn_str[MAX_TOKEN_STRING_SIZE];
	char *token_type;
	switch (token.type) {
	case TOKEN_INVALID:
		token_type = strdup("Invalid");
		break;
	case TOKEN_PLUS:
		token_type = strdup("Plus");
		break;
	case TOKEN_MINUS:
		token_type = strdup("Minus");
		break;
	case TOKEN_DIV:
		token_type = strdup("Div");
		break;
	case TOKEN_MULT:
		token_type = strdup("Mult");
		break;
		;
	case TOKEN_NUMBER:
		token_type = strdup("Number");
		break;
	case TOKEN_VARIABLE:
		token_type = strdup("Variable");
		break;
	}

	if (token.type == TOKEN_PLUS || token.type == TOKEN_MINUS ||
		token.type == TOKEN_DIV || token.type == TOKEN_MULT ||
		token.type == TOKEN_VARIABLE) {
		char *val = (char *)token.value;
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), value: %s }", token_type,
				 token.location.line, token.location.col, val);
	} else if (token.type == TOKEN_NUMBER) {
		int *val = (int *)token.value;
		snprintf(tkn_str, MAX_TOKEN_STRING_SIZE,
				 "Token{ type: %s, location: (%i, %i), value: %d }", token_type,
				 token.location.line, token.location.col, *val);
	} else if (token.type == TOKEN_INVALID) {
		char *val = (char *)token.value;
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), value: %s }", token_type,
				 token.location.line, token.location.col, val);
	}

	free(token_type);
	return tkn_str;
}

void create_token(Token *token, TokenType type, TokenLocation loc, int length,
				  char *text, void *value) {

	token->type = type;
	token->location = loc;
	token->length = length;
	token->text = text;
	token->value = value;
}
