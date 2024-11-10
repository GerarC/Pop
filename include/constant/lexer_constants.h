#ifndef LEXER_CONSTANTS_H
#define LEXER_CONSTANTS_H

#define LEXER_BEGIN_MESSAGE "Lexical analysis"
#define LEXER_TOKEN_END_OF_LINE "eln"
#define LEXER_TOKEN_END_OF_FILE "eof"

// ERROR CONSTANTS
#define LEXER_ERROR_MESSAGE_TEMPLATE "%s %i,%i"
#define LEXER_NOT_VALID_ESCAPED_CHAR_MESSAGE "not a valid escaped char"
#define LEXER_NOT_VALID_CHAR_MESSAGE "not a valid char"

// LEXER DEBUG CONSTANTS
#define LEXER_DEBUG_TOKEN_STRING_TEMPLATE "\t%s\n"
#define LEXER_DEBUG_PRINT_MESSAGE "Lexical Analysis:"
#define LEXER_DEBUG_CLEANED_MESSAGE "Lexer cleaned"

#endif // LEXER_CONSTANTS_H
