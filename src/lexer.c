#include "../include/lexer.h"
#include "../include/log.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lex_program(const char *source, const char **program, int length) {
	log_info("Lexing initialized");
	Token *tokens = NULL;
	int token_count = 0;
	int tok_num_ln;

	for (int i = 0; i < length; i++) {
		tok_num_ln = 0;
		Token *line_tokens = lex_line(source, i, program[i], &tok_num_ln);
		token_count += tok_num_ln;
		if (token_count == 0) {
			tokens = (Token *)malloc(sizeof(Token) * tok_num_ln);
		} else {
			tokens = (Token *)realloc(tokens, sizeof(Token) * token_count);
		}
		for (int j = token_count - tok_num_ln; j < token_count; j++) {
			tokens[j] = line_tokens[j % tok_num_ln];
		}
        free(line_tokens);
	}

	for (int i = 0; i < token_count; i++) {
		log_debug("%s", token_string(tokens[i]));
	}

	log_info("Lexing finished");
	return;
}

Token *lex_line(const char *source, int row, const char *line,
				int *token_count) {
	Token *tok;
	Token *tokens = NULL;
	const char *start = NULL;
	const char *curr = line;
	int col = 0;
	*token_count = 0;
	TokenLocation loc;
	loc.line = row;
	strlcpy(loc.file, source, MAX_NAME_SIZE);

	while (*curr != '\0') {
		(*token_count)++;
		if (*token_count == 0) tokens = (Token *)malloc(sizeof(Token));
		else
			tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));

		if (isdigit(*curr)) {
			// To avoid to explain all token that will be something similar I
			// prefer to explain just this one
            
			/*Count how many chars it has*/
			start = curr;
			loc.col = col;
			while (isdigit(*curr)) {
				curr++;
				col++;
			}

			/*Parse the token to its required value*/
			int val_len = curr - start;
			char val_char[val_len + 1];
			strncpy(val_char, start, val_len);
			val_char[val_len] = '\0';
			int *val = (int *)malloc(sizeof(int));
			*val = atoi(val_char);

			/*Create a token and put it in the list*/
			create_token(&(tokens[*token_count - 1]), TOKEN_INT, loc,
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
	case TOKEN_INT:
		token_type = strdup("Integer");
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
	} else if (token.type == TOKEN_INT) {
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
