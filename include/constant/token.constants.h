#ifndef TOKEN_CONSTANTS_H
#define TOKEN_CONSTANTS_H

#define TOKEN_STRING_TOK_TEMPLATE "Tk(type: [%s]\tloc: (%i:%i)\tlex: '%s')"

#define TOKEN_TYPE_TEXT_INVALID "Invalid"
#define TOKEN_TYPE_TEXT_MATH_OP "Math Op 'x'"
#define TOKEN_TYPE_TEXT_INTEGER "Integer"
#define TOKEN_TYPE_TEXT_FLOAT "Float"
#define TOKEN_TYPE_TEXT_OPEN_GROUP "OpGroup 'x'"
#define TOKEN_TYPE_TEXT_CLOSE_GROUP "ClGroup 'x'"
#define TOKEN_TYPE_TEXT_ASSIGN "Assign 'x'"
#define TOKEN_TYPE_TEXT_UNITARY_OPERATION "Move one 'xx'"
#define TOKEN_TYPE_TEXT_IDENTIFIER "Identifier"
#define TOKEN_TYPE_TEXT_PRIMITIVE "Primitive"
#define TOKEN_TYPE_TEXT_BOOLEAN "Boolean"
#define TOKEN_TYPE_TEXT_CHAR "Character"
#define TOKEN_TYPE_TEXT_COMMA "Comma"
#define TOKEN_TYPE_TEXT_COMPARATOR "Comp 'xx'"
#define TOKEN_TYPE_TEXT_LOGICAL "Logic 'xx'"
#define TOKEN_TYPE_TEXT_SINGLE_COMPARATOR "Compar 'x'"
#define TOKEN_TYPE_TEXT_BINARY_OPERATOR "Binary 'x'"
#define TOKEN_TYPE_TEXT_LOGICAL_NOT "Logic 'not'"
#define TOKEN_TYPE_TEXT_ELN "Line's end"
#define TOKEN_TYPE_TEXT_EOF "File's end"
#define TOKEN_TYPE_TEXT_RETURN "Return"
#define TOKEN_TYPE_TEXT_PRINT "Print"

#define TOKEN_TYPE_TEXT_NOT_IMPLEMENTED "NOT IMPLEMENTED"

// LOGGING MESSAGE
#define TOKEN_DEBUG_NOT_TOKEN_TO_CLEAN_MESSAGE "Token to free should exist"

#endif // TOKEN_CONSTANTS_H

