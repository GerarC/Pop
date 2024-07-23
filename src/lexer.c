#include "../include/lexer.h"
#include "../include/log.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lex_program(Lexer *lexer, const char *program) {
	log_info("Lexical analysis");

	// Loop needed
	const char *curr = program;
	const char *start;

	TokenType tok_type;
	SourceLocation loc = {0};
	strlcpy(loc.file, lexer->source, MAX_FILENAME_SIZE);
	int col = 0;

	while (*curr != '\0') {
		start = curr;
		tok_type = TOK_INVALID;

		if (*curr == '\n') {
			loc.col = col;
			curr++;
			char *lex = strdup("eln");

			if (col == 0) {
				loc.line++;
				continue;
			}

			add_token(lexer, TOK_ELN, loc, curr - start, lex, NULL);
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
			add_token(lexer, tok_type, loc, curr - start, lexeme, val);

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

			add_token(lexer, tok_type, loc, curr - start, lexeme, NULL);
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

			add_token(lexer, tok_type, loc, curr - start, lexeme, NULL);
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

			add_token(lexer, tok_type, loc, curr - start, lexeme, NULL);

		} else if (*curr == '~') {
			tok_type = TOK_BINNOT;
			curr++;
			col++;

			int len = curr - start;
			char *lex = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lex, start, len);
			lex[len] = '\0';

			add_token(lexer, tok_type, loc, curr - start, lex, NULL);

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

			add_token(lexer, tok_type, loc, curr - start, lex, NULL);

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

			add_token(lexer, tok_type, loc, curr - start, lex, NULL);
		} else if (isalpha(*curr) || *curr == '_') {
			start = curr;
			loc.col = col;
			char *lex = NULL;
			if (is_reser(&curr, "true", &lex, &col) ||
				is_reser(&curr, "false", &lex, &col))
				tok_type = TOK_BOOL;
			else if (is_reser(&curr, "and", &lex, &col)) tok_type = TOK_AND;
			else if (is_reser(&curr, "or", &lex, &col)) tok_type = TOK_OR;
			else if (is_reser(&curr, "not", &lex, &col)) tok_type = TOK_NOT;
			else if (is_reser(&curr, "if", &lex, &col)) tok_type = TOK_IF;
			else if (is_reser(&curr, "else", &lex, &col)) tok_type = TOK_ELSE;
			else if (is_reser(&curr, "while", &lex, &col)) tok_type = TOK_WHILE;
			else if (is_reser(&curr, "null", &lex, &col)) tok_type = TOK_NULL;
			else if (is_reser(&curr, "int", &lex, &col)) tok_type = TOK_INTTYPE;
			else if (is_reser(&curr, "long", &lex, &col))
				tok_type = TOK_LONGTYPE;
			else if (is_reser(&curr, "void", &lex, &col))
				tok_type = TOK_VOIDTYPE;
			else if (is_reser(&curr, "bool", &lex, &col))
				tok_type = TOK_BOOLTYPE;
			else if (is_reser(&curr, "char", &lex, &col))
				tok_type = TOK_CHARTYPE;
			else if (is_reser(&curr, "string", &lex, &col))
				tok_type = TOK_STRTYPE;
			else if (is_reser(&curr, "print_int", &lex, &col))
				tok_type = TOK_PRINT_INT;
			else if (is_reser(&curr, "print_char", &lex, &col))
				tok_type = TOK_PRINT_CHAR;
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

			add_token(lexer, tok_type, loc, strlen(lex), lex, NULL);

		} else if (*curr == '\'') {
			curr++;
			col++;
			tok_type = TOK_CHAR;
			char *lex = (char *)malloc(sizeof(char) * 2);
			lex[1] = '\0';
			if ((int)*curr < 128) {
				start = curr;
				lex[0] = *start;
				if (*curr == '\\') {
					log_trace("enters");
					curr++;
					col++;
					if (*curr == 'n') lex[0] = '\n';
					else if (*curr == 't') lex[0] = '\t';
					else if (*curr == 'r') lex[0] = '\r';
					else if (*curr == '0') lex[0] = '\0';
					else {
						log_fatal("not a valid escaped char %i,%i", loc.line,
								  col);
						exit(1);
					}
				}
				curr++;
				col++;
				if (*curr != '\'') {
					log_fatal("not a valid char %i,%i", loc.line, col);
					exit(1);
				}
				curr++;
				col++;
			} else {
				log_fatal("not a valid char");
				exit(1);
			}

			add_token(lexer, tok_type, loc, 1, lex, NULL);

		} else if (*curr == ',') {
			curr++;
			col++;
			tok_type = TOK_COMMA;

			int len = curr - start;
			char *lex = (char *)malloc(sizeof(char) * len + 1);
			strncpy(lex, start, len);
			lex[len] = '\0';

			add_token(lexer, tok_type, loc, curr - start, lex, NULL);

		} else if (isspace(*curr)) {
			while (isspace(*curr) && *curr != '\n') {
				curr++;
				col++;
			}

		} else if (*curr == '/') {
		lex_program_comment:
			while (*curr != '\n')
				curr++;
			if (col == 1) curr++;
			col = 0;
			loc.line++;
		} else {
			loc.col = col;
			curr++;
			col++;

			char *lex = (char *)malloc(sizeof(char) * 2);
			strncpy(lex, start, 1);
			lex[1] = '\0';

			add_token(lexer, TOK_INVALID, loc, curr - start, lex, NULL);
		}
	}

	add_token(lexer, TOK_EOF, loc, 1, strdup("eof"), NULL);
}

int is_reser(const char **curr, const char *rword, char **dest, int *col) {
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

void add_token(Lexer *lexer, TokenType type, SourceLocation loc, int len,
			   char *lex, void *val) {
	if (lexer->count >= lexer->capacity) {
		lexer->capacity *= 2;
		lexer->tokens =
			(Token *)realloc(lexer->tokens, sizeof(Token) * lexer->capacity);
	}

	lexer->tokens[lexer->count].type = type;
	lexer->tokens[lexer->count].location = loc;
	lexer->tokens[lexer->count].length = len;
	lexer->tokens[lexer->count].lexeme = lex;
	lexer->tokens[lexer->count].value = val;

	lexer->count++;
}

void print_lexer(Lexer *lex) {
	if (LOG_DEBUG < LOG_LEVEL) return;
	log_debug("Lexical Analysis:");
	for (int i = 0; i < lex->count; i++)
		printf("\t%s\n", token_string(lex->tokens[i]));
}

void free_lexer(Lexer lex) {
	for (int i = 0; i < lex.count; i++)
		free_token(&lex.tokens[i]);
	free(lex.tokens);
	log_info("lexer cleaned");
}

Lexer create_lexer(const char *source) {
	Lexer lex = {.count = 0,
				 .capacity = 1,
				 .tokens = (Token *)malloc(sizeof(Token) * 1)};
	strlcpy(lex.source, source, MAX_FILE_SIZE);

	return lex;
}
