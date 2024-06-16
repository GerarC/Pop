#ifndef COMMON_H
#define COMMON_H

#define MAX_NAME_SIZE 128
#define MAX_TOK_STRING_SIZE 256

/* Enum with all the kinds of tokens
 * */
typedef enum _token_type_e {
	TOK_SYMBOL, // Variables
	TOK_NUM,	// Number
	TOK_STR,	// String
	TOK_BOOL,	// String
	TOK_ELN,	// EndLine
	TOK_EOF,	// End of File
				// Comparators
	TOK_NOT,// ! not
	TOK_GT,	   // >
	TOK_GEQT,  // >=
	TOK_LT,	   // <
	TOK_LEQT,  // <=
	TOK_EQUAL, // ==
	TOK_DIFF,  // ==
	TOK_OR,   //  || or
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
	TOK_PLUS,  // +
	TOK_MINUS, // -
	TOK_SLASH, // /
	TOK_STAR,  // * and pointers
	TOK_MOD,   // %
	TOK_BINOR,   //  |
	TOK_BINAND,   // &
	TOK_BINXOR,   // ^
	// Reserved
	TOK_NEW,	  // New
	TOK_CLASS,	  // Class
	TOK_LET,	  // Variable
	TOK_CONST,	  // Constant
	TOK_UNSIGNED, // Constant
	TOK_ARROW,	  // ->
	TOK_IF,		  // if
	TOK_ELSE,	  // if
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

	// Invalid
	TOK_INVALID
} TokenType;

/* Structure that saves the exact location of a token in the file
 * */
typedef struct _token_location_t {
	int line, col;
	char file[MAX_NAME_SIZE];
} TokenLocation;

/* Token structure represents a piece of code in its most important information
 * */
typedef struct _token_t {
	TokenType type;
	TokenLocation location;
	int length;
	char *lexeme;
	void *value;
} Token;

/* Create a new token with the most important information
 * */
void create_token(Token *token, TokenType type, TokenLocation loc, int length,
				  char *text, void *value);

/* Takes a token a return a string representation
 * */
char *token_string(Token tok);

/* As token saves so many things then it's necessary to use this function in
 * order too free that mem at the final of the procecss;
 * */
void free_token(Token *tok);

#endif // !COMMON_H
