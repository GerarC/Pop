#ifndef COMMON_H
#define COMMON_H

#include "common.h"
#include "config.h"

/* Enum with all the kinds of tokens
 * */
typedef enum _token_type_e {
	TOK_IDENTIFIER, // Variables
	TOK_INT,		// Integer
	TOK_LONG,		// Long
	TOK_CHAR,		// char
	TOK_STR,		// Str
	TOK_BOOL,		// Booleans
	TOK_FLOAT,		// Float
	TOK_IMAGINARY,	// Imaginary
	TOK_ELN,		// EndLine
	TOK_MAIN,		// Root of the file
	TOK_EOF,		// End of File
			 // Comparators
	TOK_NOT,   // ! not
	TOK_GT,	   // >
	TOK_GEQT,  // >=
	TOK_LT,	   // <
	TOK_LEQT,  // <=
	TOK_EQUAL, // ==
	TOK_DIFF,  // ==
	TOK_OR,	   //  || or
	TOK_AND,   // && and
	TOK_XOR,   // ^ xor

	// Assignment
	TOK_PLUS_PLUS,	 // ++
	TOK_MINUS_MINUS, // --
	TOK_PLUS_EQUAL,	 // +=
	TOK_MINUS_EQUAL, // -=
	TOK_STAR_EQUAL,	 // *=
	TOK_SLASH_EQUAL, // /=
	TOK_ASSIGN,		 // =
	// Math
	TOK_PLUS,	// +
	TOK_MINUS,	// -
	TOK_SLASH,	// /
	TOK_STAR,	// * and pointers
	TOK_MOD,	// %
	TOK_BINNOT, // ! not
	TOK_BINOR,	//  |
	TOK_BINAND, // &
	TOK_BINXOR, // ^
	// Reserved
	TOK_NEW,		// New
	TOK_CLASS,		// Class
	TOK_LET,		// Variable
	TOK_CONST,		// Constant
	TOK_UNSIGNED,	// Constant
	TOK_ARROW,		// ->
	TOK_IF,			// if
	TOK_ELSE,		// if
	TOK_WHILE,		// if
	TOK_BLOCK_GLUE, // NO TOKEN JUST A GLUE
	TOK_ARGS_GLUE,	// NO TOKEN JUST A GLUE
	// TOK_IMPORT, // Constant
	// Grouping
	TOK_LPAREN, // (
	TOK_RPAREN, // )
	TOK_LCURLY, // {
	TOK_RCURLY, // }
	TOK_LBRACE, // [
	TOK_RBRACE, // ]
	TOK_LANGLE, // <
	TOK_RANGLE, // >
				// separators
	TOK_COMMA,	// ,
	// TYPES
	TOK_INTTYPE,  // 'int'
	TOK_LONGTYPE, // 'long'
	TOK_BOOLTYPE, // 'bool'
	TOK_CHARTYPE, // 'char'
	TOK_STRTYPE,  // 'string'
	TOK_VOIDTYPE, // 'void'
	TOK_NULL,	  // 'null'

	TOK_PRINT_INT,
	TOK_PRINT_CHAR,
	// Invalid
	TOK_INVALID
} TokenType;

/* Token structure represents a piece of code in its most important information
 * */
typedef struct _token_t {
	TokenType type;
	SourceLocation location;
	int length;
	char *lexeme;
	void *value;
} Token;

/* Create a new token with the most important information
 * */
void create_token(Token *token, TokenType type, SourceLocation loc, int length,
				  char *lexeme, void *value);

/* Takes a token a return a string representation
 * */
char *token_string(Token tok);

/* As token saves so many things then it's necessary to use this function in
 * order too free that mem at the final of the procecss;
 * */
void free_token(Token *tok);

#endif // !COMMON_H
