#ifndef INTERMEDIARY_REPRESENTATION_H
#define INTERMEDIARY_REPRESENTATION_H

#include "parser.h"

typedef enum {
    IR_VALUE,
	IR_ADD,
	IR_SUB,
	IR_MUL,
	IR_DIV,
} IrOperationType;

typedef enum {
	IRVAL_INT,
	IRVAL_FLOAT,
	IRVAL_INDEX,
} IrValueType;

typedef struct _ir_value_t {
	IrValueType type;
	union Data {
		int ival;
		float fval;
		int index;
	} data;
} IrValue;

typedef struct _ir_operation_t {
	IrOperationType type;
	IrValue arg1; // can be null
	IrValue arg2; // can be null
} IrOperation;

typedef struct _intermediary_representation_t {
	IrOperation *instructions;
	int count;
	int capacity;
} IntermediateRepresentation;

IntermediateRepresentation *create_intermediate_representation(Node *ast);

void free_intermediate_representation(IntermediateRepresentation *ir);

void print_ir(IntermediateRepresentation *ir);

#endif // !INTERMEDIARY_REPRESENTATION_H
