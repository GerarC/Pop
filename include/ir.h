#ifndef INTERMEDIARY_REPRESENTATION_H
#define INTERMEDIARY_REPRESENTATION_H

#include "config.h"
#include "parser.h"
#include "symboltable.h"

typedef enum {
	IR_ADD,
	IR_SUB,
	IR_MUL,
	IR_DIV,
	IR_MOD,
	IR_UMINUS,

	IR_LT,
	IR_LEQT,
	IR_GT,
	IR_GEQT,
	IR_EQUAL,
	IR_DIFF,

	IR_NOT,
	IR_IF,
	IR_ELSE,
	IR_WHILE,
	IR_DO,

	IR_BINNOT,

	IR_ENDBLOCK,

	IR_DECLARATION,
	IR_FUNCT_DECLARATION,
	IR_FUNCT_END,
	IR_FUNCT_USAGE,
	IR_RETURN,
	IR_ASSIGNMENT,

	IR_TEMP_PRINT_INT,
	IR_TEMP_PRINT_CHAR,
} IrOperationType;

typedef enum {
	IRVAL_INT,
	IRVAL_CHAR,
	IRVAL_FLOAT,
	IRVAL_ADDRESS,
	IRVAL_IDENTIFIER,
} IrValueType;

typedef struct _ir_value_t {
	IrValueType type;
	union Data {
		int ival;
		float fval;
		char cval;
		size index;
		char ident[MAX_SYMBOL_SIZE];
	} data;
} IrValue;

typedef struct _ir_operation_t {
	IrOperationType type;
	IrValue arg1;	// can be null
	IrValue arg2;	// can be null
	IrValue result; // can be null
    int is_used;
} IrOperation;

typedef struct _intermediary_representation_t {
	IrOperation *instructions;
	size count;
	size capacity;
	struct _intermediary_representation_t *globals;
} IntermediateRepresentation;

IntermediateRepresentation *create_intermediate_representation(Node *ast, SymbolTable *tbl);

void free_intermediate_representation(IntermediateRepresentation *ir);

void print_ir(IntermediateRepresentation *ir);

#endif // !INTERMEDIARY_REPRESENTATION_H
