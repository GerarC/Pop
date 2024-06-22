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
		int base =
			token_count == tok_num_ln ? token_count : token_count - tok_num_ln;
		for (int j = token_count - tok_num_ln; j < token_count; j++)
			tokens[j] = line_tokens[j % base];
		free(line_tokens);
	}

	token_count++;
	if (token_count == 0) tokens = (Token *)malloc(sizeof(Token) * 1);
	else tokens = (Token *)realloc(tokens, sizeof(Token) * token_count);
	TokenLocation loc = {-1, -1};
	strlcpy(loc.file, source, MAX_NAME_SIZE);
	create_token(&tokens[token_count - 1], TOK_EOF, loc, 1, NULL, NULL);

	/* for (int i = 0; i < token_count; i++)
			log_debug("%i\t%s", i, token_string(tokens[i])); */

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
		if (tokens == NULL) tokens = (Token *)malloc(sizeof(Token) * 1);
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
			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			int *val = (int *)malloc(sizeof(int));
			*val = atoi(lexeme);

			/*Create a token and put it in the list*/
			create_token(&(tokens[*token_count - 1]), TOK_NUM, loc,
						 curr - start, lexeme, val);
		} else if (isspace(*curr)) {
			while (isspace(*curr)) {
				curr++;
				col++;
			}
			(*token_count)--;
		} else if (*curr == '+' || *curr == '-' || *curr == '*' ||
				   *curr == '/' || *curr == '%') {
			// Operations
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
				curr++;
				col++;
			} else if (*curr == '/') goto ll_comment;
			else if (*curr == '=') goto ll_eq;

			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, lexeme, NULL);
		} else if (*curr == '<' || *curr == '>' || *curr == '!') {
			// Comparators
			if (*curr == '<') tok_type = TOK_LT;
			else if (*curr == '>') tok_type = TOK_GT;
			else if (*curr == '!') tok_type = TOK_NOT;
			else tok_type = TOK_INVALID;

			start = curr;
			loc.col = col;
			curr++;
			col++;
			if (*curr == '=') {
				if (tok_type == TOK_LT) tok_type = TOK_LEQT;
				else if (tok_type == TOK_GT) tok_type = TOK_GEQT;
				else if (tok_type == TOK_NOT) tok_type = TOK_DIFF;
				curr++;
				col++;
			}

			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, lexeme, NULL);

		} else if (*curr == '|' || *curr == '&' || *curr == '^') {
			// Bynary Expressions
			if (*curr == '|') tok_type = TOK_BINOR;
			else if (*curr == '&') tok_type = TOK_BINAND;
			else if (*curr == '^') tok_type = TOK_BINXOR;
			else tok_type = TOK_INVALID;

			start = curr;
			loc.col = col;
			curr++;
			col++;

			if (*curr == '|' || *curr == '&') {
				if (tok_type == TOK_BINAND) tok_type = TOK_AND;
				else if (tok_type == TOK_BINOR) tok_type = TOK_OR;
				else tok_type = TOK_INVALID;
				curr++;
				col++;
			}

			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, lexeme, NULL);

		} else if (*curr == '=') {
		ll_eq:
			if (tok_type == TOK_PLUS) tok_type = TOK_PLUS_EQUAL;
			else if (tok_type == TOK_MINUS) tok_type = TOK_MINUS_EQUAL;
			else if (tok_type == TOK_STAR) tok_type = TOK_STAR_EQUAL;
			else if (tok_type == TOK_SLASH) tok_type = TOK_SLASH_EQUAL;
			else tok_type = TOK_ASSIGN;

			if (start == NULL) {
				start = curr;
				loc.col = col;
			}
			curr++;
			col++;
			if (tok_type == TOK_ASSIGN && *curr == '=') {
				tok_type = TOK_EQUAL;
				curr++;
				col++;
			}

			int len = curr - start;
			char *lex = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lex, start, len);
			lex[len] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);
		} else if (*curr == '/') {
		ll_comment:
			while (*curr != '\0')
				curr++;
			(*token_count)--;

		} else if (isalpha(*curr) || *curr == '_') {
			start = curr;
			loc.col = col;
			char *lex = NULL;
			if (compare_reserved(&curr, "true", &lex, &col) ||
				compare_reserved(&curr, "false", &lex, &col))
				tok_type = TOK_BOOL;
			else if (compare_reserved(&curr, "and", &lex, &col))
				tok_type = TOK_AND;
			else if (compare_reserved(&curr, "or", &lex, &col))
				tok_type = TOK_OR;
			else if (compare_reserved(&curr, "if", &lex, &col))
				tok_type = TOK_IF;
			else if (compare_reserved(&curr, "new", &lex, &col))
				tok_type = TOK_NEW;
			else tok_type = TOK_SYMBOL;

			if ((isalpha(*curr) || isdigit(*curr) || *curr == '_') &&
				tok_type != TOK_SYMBOL) {
				tok_type = TOK_SYMBOL;
				free(lex);
			}

			if (tok_type == TOK_SYMBOL) {
				while (isalpha(*curr) || isdigit(*curr) || *curr == '_') {
					curr++;
					col++;
				}
				int len = curr - start;
				lex = (char *)malloc(sizeof(char) * len + 1);
				strncpy(lex, start, len);
				lex[len] = '\0';
			}

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 strlen(lex), lex, NULL);
		} else if (*curr == '(' || *curr == ')' || *curr == '{' ||
				   *curr == '}' || *curr == '[' || *curr == ']' ||
				   *curr == '<' || *curr == '>') { // Math simbols
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

			char *lex = (char *)malloc(sizeof(char) * 2);
			strncpy(lex, start, 1);
			lex[1] = '\0';

			create_token(&(tokens[*token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);
		} else {
			start = curr;
			loc.col = col;
			curr++;
			col++;

			char *lex = (char *)malloc(sizeof(char) * 2);
			strncpy(lex, start, 1);
			lex[1] = '\0';

			create_token(&(tokens[*token_count - 1]), TOK_INVALID, loc,
						 curr - start, lex, NULL);
		}
	}

	if (*token_count > 0) {
		(*token_count)++;
		tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));
		char *lex = strdup("eln");
		create_token(&(tokens[*token_count - 1]), tok_type, loc, curr - start,
					 lex, NULL);
	}

	return tokens;
}

int compare_reserved(const char **curr, const char *rword, char **dest,
					 int *col) {
	int equals = 0;
	int rwlen = strlen(rword);
	if (strlen(*curr) >= rwlen) {
		for (int i = 0; i < rwlen; i++)
			if ((*curr)[i] != rword[i]) return 0;
	} else return 0;

	*dest = strdup(rword);
	(*curr) += rwlen;
	(*col) += rwlen;
	return 1;
}

void free_lexer(Token *tokens, int len) {
	for (int i = 0; i < len; i++) {
		free_token(tokens + i);
	}
	free(tokens);

	log_info("lexer cleaned");
}
