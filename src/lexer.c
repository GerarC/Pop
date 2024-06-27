#include "../include/lexer.h"
#include "../include/log.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *lex_line(const char *source, int row, const char *line,
				int *token_count);

int compare_reserved(const char **curr, const char *rword, char **dest,
					 int *col);

Token *lex_program(const char *source, const char *program, int *lexer_len) {
	log_info("Lexing initialized");
	Token *tokens = NULL;
	TokenType tok_type;
	int token_count = 0;

	// Loop needed
	const char *curr = program;
	const char *start;
	TokenLocation loc = {0};
	strlcpy(loc.file, source, MAX_FILENAME_SIZE);
	int col = 0;

	while (*curr != '\0') {
		token_count++;
		start = curr;
		tok_type = TOK_INVALID;
		if (tokens == NULL) tokens = (Token *)malloc(sizeof(Token) * 1);
		else tokens = (Token *)realloc(tokens, sizeof(Token) * token_count);

		if (*curr == '\n') {
			loc.col = col;
			curr++;
			char *lex = strdup("eln");

			if (col == 0) {
				token_count--;
				loc.line++;
				continue;
			}

			create_token(&(tokens[token_count - 1]), TOK_ELN, loc, curr - start,
						 lex, NULL);
			col = 0;
			loc.line++;
		} else if (isdigit(*curr)) {
			// To avoid to explain all token that will be something similar I
			// prefer to explain just this one

			/*Count how many chars it has*/
			loc.col = col;
			tok_type = TOK_INT;
			while (isdigit(*curr)) {
				curr++;
				col++;
			}

			if (*curr == '.' || *curr == 'E') { // It's a float
				tok_type = TOK_FLOAT;
				curr++;
				col++;
				while (isdigit(*curr)) {
					curr++;
					col++;
				}
			}

			/*Parse the token to its required value*/
			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			int *val = (int *)malloc(sizeof(int));
			*val = atoi(lexeme);

			/*Create a token and put it in the list*/
			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lexeme, val);
		} else if (*curr == '+' || *curr == '-' || *curr == '*' ||
				   *curr == '/' || *curr == '%') {
			// Operations
			if (*curr == '+') tok_type = TOK_PLUS;
			else if (*curr == '-') tok_type = TOK_MINUS;
			else if (*curr == '*') tok_type = TOK_STAR;
			else if (*curr == '/') tok_type = TOK_SLASH;
			else if (*curr == '%') tok_type = TOK_MOD;
			else tok_type = TOK_INVALID;

			loc.col = col;
			curr++;
			col++;

			if (*curr == '+' || *curr == '-') {
				if (*curr == '+' && tok_type == TOK_PLUS)
					tok_type = TOK_PLUS_PLUS;
				else if (*curr == '-' && tok_type == TOK_MINUS)
					tok_type = TOK_MINUS_MINUS;
				else tok_type = TOK_INVALID;
				curr++;
				col++;
			} else if (*curr == '/' && *start == '/') goto lex_program_comment;
			else if (*curr == '=') goto lex_program_eq;

			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			create_token(&(tokens[token_count - 1]), tok_type, loc,
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

			create_token(&(tokens[token_count - 1]), tok_type, loc,
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

			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lexeme, NULL);

		} else if (*curr == '=') {
		lex_program_eq:
			if (tok_type == TOK_PLUS) tok_type = TOK_PLUS_EQUAL;
			else if (tok_type == TOK_MINUS) tok_type = TOK_MINUS_EQUAL;
			else if (tok_type == TOK_STAR) tok_type = TOK_STAR_EQUAL;
			else if (tok_type == TOK_SLASH) tok_type = TOK_SLASH_EQUAL;
			else {
				tok_type = TOK_ASSIGN;
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

			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);

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

			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);
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
			else if (compare_reserved(&curr, "not", &lex, &col))
				tok_type = TOK_NOT;
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

			create_token(&(tokens[token_count - 1]), tok_type, loc, strlen(lex),
						 lex, NULL);
		} else if (isspace(*curr)) {
			while (isspace(*curr)) {
				curr++;
				col++;
			}
			token_count--;

		} else if (*curr == '/') {
		lex_program_comment:
			while (*curr != '\n')
				curr++;
			if (col == 1) curr++;
			col = 0;
			loc.line++;
			token_count--;
		} else {
			loc.col = col;
			curr++;
			col++;

			char *lex = (char *)malloc(sizeof(char) * 2);
			strncpy(lex, start, 1);
			lex[1] = '\0';

			create_token(&(tokens[token_count - 1]), TOK_INVALID, loc,
						 curr - start, lex, NULL);
		}
	}

	token_count++;
	if (tokens == NULL) tokens = (Token *)malloc(sizeof(Token) * 1);
	else tokens = (Token *)realloc(tokens, sizeof(Token) * token_count);
	create_token(&(tokens[token_count - 1]), TOK_EOF, loc, 1, strdup("eof"),
				 NULL);

	/*for (int i = 0; i < token_count; i++) log_trace("%i\t%s", i,
	 * token_string(tokens[i]));*/

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
	strlcpy(loc.file, source, MAX_FILENAME_SIZE);

	while (*curr != '\0') {
		log_trace("current -> %c", *curr);
		(*token_count)++;
		tok_type = TOK_INVALID;
		start = NULL;
		if (tokens == NULL) tokens = (Token *)malloc(sizeof(Token) * 1);
		else tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));

		if (isdigit(*curr)) {
		} else if (isspace(*curr)) {
		} else if (*curr == '+' || *curr == '-' || *curr == '*' ||
				   *curr == '/' || *curr == '%') {
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
			else if (compare_reserved(&curr, "not", &lex, &col))
				tok_type = TOK_NOT;
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
			log_fatal("Token %c is invalid", *start);
		}
	}

	if (*token_count > 0) {
		(*token_count)++;
		tokens = (Token *)realloc(tokens, sizeof(Token) * (*token_count));
		char *lex = strdup("eln");
		create_token(&(tokens[*token_count - 1]), TOK_ELN, loc, curr - start,
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
