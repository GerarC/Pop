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
	Token *tokens = (Token *)malloc(sizeof(Token) * 1);
	TokenType tok_type;
	int token_count = 0;
	int capacity = 1;

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
		if (token_count >= capacity) {
			capacity *= 2;
			tokens = (Token *)realloc(tokens, sizeof(Token) * capacity);
		}

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
				int e_notation = 0;
			lex_program_float_E_notation:
				curr++;
				col++;
				while (isdigit(*curr)) {
					curr++;
					col++;
				}
				if (*curr == 'E' && !e_notation) {
					curr++;
					col++;
					goto lex_program_float_E_notation;
				}
			}

			/*Parse the token to its required value*/
			int len = curr - start;
			char *lexeme = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lexeme, start, len);
			lexeme[len] = '\0';

			void *val;
			if (tok_type == TOK_FLOAT) {
				float *fval = malloc(sizeof(float));
				*fval = atof(lexeme);
				val = fval;
			} else {
				int *ival = malloc(sizeof(int));
				*ival = atof(lexeme);
				val = ival;
			}

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

		} else if (*curr == '~') {
			tok_type = TOK_BINNOT;
			curr++;
			col++;

			int len = curr - start;
			char *lex = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lex, start, len);
			lex[len] = '\0';

			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);

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
			else if (compare_reserved(&curr, "else", &lex, &col))
				tok_type = TOK_ELSE;
			else if (compare_reserved(&curr, "while", &lex, &col))
				tok_type = TOK_WHILE;
			else if (compare_reserved(&curr, "int", &lex, &col))
				tok_type = TOK_INTTYPE;
			else if (compare_reserved(&curr, "int", &lex, &col))
				tok_type = TOK_BOOLTYPE;
			else if (compare_reserved(&curr, "print_int", &lex, &col))
				tok_type = TOK_PRINT_INT;
			else tok_type = TOK_IDENTIFIER;

			if ((isalpha(*curr) || isdigit(*curr) || *curr == '_') &&
				tok_type != TOK_IDENTIFIER) {
				tok_type = TOK_IDENTIFIER;
				free(lex);
			}

			if (tok_type == TOK_IDENTIFIER) {
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
		} else if (*curr == ',') {
			curr++;
			col++;
			tok_type = TOK_COMMA;

			int len = curr - start;
			char *lex = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lex, start, len);
			lex[len] = '\0';

			create_token(&(tokens[token_count - 1]), tok_type, loc,
						 curr - start, lex, NULL);

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
	if (token_count >= capacity) {
		capacity *= 2;
		tokens = (Token *)realloc(tokens, sizeof(Token) * capacity);
	}
	create_token(&(tokens[token_count - 1]), TOK_EOF, loc, 1, strdup("eof"),
				 NULL);

	/*for (int i = 0; i < token_count; i++) log_trace("%i\t%s", i,
	 * token_string(tokens[i]));*/

	log_info("Lexing finished");
	*lexer_len = token_count;
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
