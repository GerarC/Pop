#include "../include/lexer.h"
#include "../include/log.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *lex_program(const char *source, const char **program, int length,
				   int *lexer_len) {
	log_info("Lexing initialized");
	log_warn(
		"Remember to implement all the types in the To_String token function");
	Token *tokens = NULL;
	int token_count = 0;
	int tok_num_ln;

	for (int i = 0; i < length; i++) {
		tok_num_ln = 0;
		Token *line_tokens = lex_line(source, i, program[i], &tok_num_ln);
		token_count += tok_num_ln;
		if (token_count == 0) tokens = (Token *)malloc(sizeof(Token) * 1);
		else tokens = (Token *)realloc(tokens, sizeof(Token) * token_count);
		for (int j = token_count - tok_num_ln; j < token_count; j++) {
			tokens[j] = line_tokens[j % tok_num_ln];
		}
		free(line_tokens);
	}
	for (int i = 0; i < token_count; i++) {
		log_debug("%i\t%s", i, token_string(tokens[i]));
	}

	log_info("Lexing finished");
	*lexer_len = token_count;
	return tokens;
}

Token *lex_line(const char *source, int row, const char *line,
				int *token_count) {
	Token *tok;
	Token *tokens = NULL;
	const char *start = NULL;
	const char *curr = line;
	int col = 0;
	TokenLocation loc;
	loc.line = row;
	TokenType tok_type;
	*token_count = 0;
	strlcpy(loc.file, source, MAX_NAME_SIZE);

	while (*curr != '\0') {
		(*token_count)++;
		tok_type = TOK_INVALID;
		start = NULL;
		if (*token_count == 0) tokens = (Token *)malloc(sizeof(Token));
		else tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));

		if (isdigit(*curr)) {
			// To avoid to explain all token that will be something similar I
			// prefer to explain just this one

			/*Count how many chars it has*/
			start = curr;
			loc.col = col;
		ll_int:
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
			create_token(&(tokens[*token_count - 1]), TOK_NUM, loc,
						 curr - start, NULL, val);
		} else if (isspace(*curr)) {
			while (isspace(*curr)) {
				curr++;
				col++;
			}
			(*token_count)--;
		} else if (*curr == '+' || *curr == '-' || *curr == '*' ||
				   *curr == '/' || *curr == '%') { // Math simbols
			if (*curr == '+') tok_type = TOK_PLUS;
			else if (*curr == '-') tok_type = TOK_MINUS;
			else if (*curr == '*') tok_type = TOK_STAR;
			else if (*curr == '/') tok_type = TOK_SLASH;
			else if (*curr == '%') tok_type = TOK_MOD;
			else tok_type = TOK_INVALID;

			start = curr;
			loc.col = col;
			curr++;
			col++;

			if (isdigit(*curr) && tok_type == TOK_MINUS) goto ll_int;
			else if (*curr == '+' || *curr == '-') {
				if (*curr == '+' || tok_type == TOK_PLUS)
					tok_type = TOK_PLUS_PLUS;
				else if (*curr == '-' || tok_type == TOK_MINUS)
					tok_type = TOK_MINUS_MINUS;
				else tok_type = TOK_INVALID;
				log_debug("%i %i %i", tok_type, TOK_PLUS_PLUS, TOK_MINUS_MINUS);
				curr++;
				col++;
			} else if (*curr == '/') goto ll_comment;
			else if (*curr == '=') goto ll_assign;

			int len = curr - start;
			char *val = (char *)malloc(sizeof(char) * len + 1);
			strncpy(val, start, len);
			val[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, NULL, val);
		} else if (*curr == '(' || *curr == ')' || *curr == '{' ||
				   *curr == '}' || *curr == '[' || *curr == ']' ||
				   *curr == '<' || *curr == '>') { // Math simbols
			TokenType tok_type;
			if (*curr == '(') tok_type = TOK_LPAREN;
			else if (*curr == ')') tok_type = TOK_RPAREN;
			else if (*curr == '{') tok_type = TOK_LCURLY;
			else if (*curr == '}') tok_type = TOK_RCURLY;
			else if (*curr == '[') tok_type = TOK_LBRACE;
			else if (*curr == ']') tok_type = TOK_RBRACE;
			else if (*curr == '<') tok_type = TOK_LANGLE;
			else if (*curr == '>') tok_type = TOK_RANGLE;
			else tok_type = TOK_INVALID;

			start = curr;
			loc.col = col;
			curr++;
			col++;

			char *val = (char *)malloc(sizeof(char) * 2);
			strncpy(val, start, 1);
			val[1] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, NULL, val);
		} else if (*curr == '=') {
		ll_assign:
			if (tok_type == TOK_PLUS) tok_type = TOK_PLUS_EQUAL;
			else if (tok_type == TOK_MINUS) tok_type = TOK_MINUS_EQUAL;
			else if (tok_type == TOK_STAR) tok_type = TOK_STAR_EQUAL;
			else if (tok_type == TOK_SLASH) tok_type = TOK_SLASH_EQUAL;
			else tok_type = TOK_EQUAL;

			if (start == NULL) {
				start = curr;
				loc.col = col;
			}
			curr++;
			col++;

			int len = curr - start;
			char *val = (char *)malloc(sizeof(char) * len + 1);
			strncpy(val, start, len);
			val[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, NULL, val);
		} else if (*curr == '/') {
		ll_comment:
			while (*curr != '\0')
				curr++;
			(*token_count)--;

		} else {
			start = curr;
			loc.col = col;
			curr++;
			col++;

			char *val = (char *)malloc(sizeof(char) * 2);
			strncpy(val, start, 1);
			val[1] = '\0';

			create_token(&(tokens[*token_count - 1]), TOK_INVALID, loc,
						 curr - start, NULL, val);
		}
	}

	return tokens;
}

static char *token_string(Token token) {
	static char tkn_str[MAX_TOK_STRING_SIZE];
	char *token_type;
	switch (token.type) {
		case TOK_INVALID:
			token_type = strdup("Invalid");
			break;
		case TOK_PLUS:
			token_type = strdup("Plus");
			break;
		case TOK_MINUS:
			token_type = strdup("Minus");
			break;
		case TOK_SLASH:
			token_type = strdup("Div");
			break;
		case TOK_STAR:
			token_type = strdup("Mult");
			break;
		case TOK_MOD:
			token_type = strdup("Module");
			break;
		case TOK_NUM:
			token_type = strdup("Integer");
			break;
		case TOK_LPAREN:
		case TOK_LCURLY:
		case TOK_LBRACE:
		case TOK_LANGLE:
			token_type = strdup("OpGroup 'x'");
			token_type[strlen(token_type) - 2] = ((char *)token.value)[0];
			break;
		case TOK_RPAREN:
		case TOK_RCURLY:
		case TOK_RBRACE:
		case TOK_RANGLE:
			token_type = strdup("ClGroup 'x'");
			token_type[strlen(token_type) - 2] = ((char *)token.value)[0];
			break;
		case TOK_EQUAL:
		case TOK_PLUS_EQUAL:
		case TOK_MINUS_EQUAL:
		case TOK_STAR_EQUAL:
		case TOK_SLASH_EQUAL:
			token_type = strdup("Assign 'x'");
			token_type[strlen(token_type) - 2] = ((char *)token.value)[0];
			break;
		case TOK_PLUS_PLUS:
		case TOK_MINUS_MINUS:
			token_type = strdup("Move one 'xx'");
			token_type[strlen(token_type) - 3] = ((char *)token.value)[0];
			token_type[strlen(token_type) - 2] = ((char *)token.value)[1];
			break;
		case TOK_SYMBOL:
			token_type = strdup("Variable");
			break;
		default:
			token_type = strdup("NOT IMPLEMENTED");
			break;
	}

	if (token.type == TOK_PLUS || token.type == TOK_MINUS ||
		token.type == TOK_SLASH || token.type == TOK_STAR ||
		token.type == TOK_MOD || token.type == TOK_SYMBOL) {
		char *val = (char *)token.value;
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), value: %s }", token_type,
				 token.location.line, token.location.col, val);
	} else if (token.type == TOK_LPAREN || token.type == TOK_RPAREN ||
			   token.type == TOK_LCURLY || token.type == TOK_RCURLY ||
			   token.type == TOK_LBRACE || token.type == TOK_RBRACE ||
			   token.type == TOK_LANGLE || token.type == TOK_RANGLE) {
		char *val = (char *)token.value;
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), value: %s }", token_type,
				 token.location.line, token.location.col, val);
	} else if (token.type == TOK_NUM) {
		int *val = (int *)token.value;
		snprintf(tkn_str, MAX_TOK_STRING_SIZE,
				 "Token{ type: %s, location: (%i, %i), value: %d }", token_type,
				 token.location.line, token.location.col, *val);
	} else if (token.type == TOK_INVALID) {
		char *val = (char *)token.value;
		snprintf(tkn_str, sizeof(tkn_str),
				 "Token{ type: %s, location: (%i, %i), value: %s }", token_type,
				 token.location.line, token.location.col, val);
	} else {
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

void free_token(Token *tok) {
	if (tok == NULL) {
		log_warn("Token to free should exist");
		return;
	}
	if (tok->text != NULL) free(tok->text);
	if (tok->value != NULL) free(tok->value);
	/*free(tok);*/;
}

void free_lexer(Token *tokens, int len) {
	for (int i = 0; i < len; i++) {
		free_token(tokens + i);
	}
	free(tokens);

	log_debug("lexer cleaned");
}
