#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "./common.h"
#include "./config.h"
#include "./typedef.h"

typedef enum _data_type_e {
	DT_INT,
	DT_FLOAT,
	DT_LONG,
	DT_CHAR,
} DataType;

typedef enum _structural_type_e {
	ST_VARIABLE,
	ST_FUNCTION,
	ST_ARRAY,
	ST_PARAM,
	ST_STRUCT,
	ST_CLASS,
	ST_ENUM,
	ST_MEMBER,
} StructureType;

typedef enum _symbol_attribute_e {
	SA_STATIC = (1 << 0),
	SA_CONST = (1 << 1),
} SymbolAttribute;

typedef enum _storage_class {
	SC_GLOBAL = 1,
	SC_LOCAL,
} ScopeType;

/* It's a wrapper structure to represent a symbol in the entry table
 * */
typedef struct _symbol_t {
	char name[MAX_SYMBOL_SIZE];
	SourceLocation loc;
	ScopeType scope;
	DataType dtype;
	StructureType stype;
	// attributes
	int attrs;
	int offset;
	SymbolTable *members;
} Symbol;

/* Symbol Table has a list of Entries. It means that has a list of types and the
 * symbols they belong
 * */
typedef struct _symbol_table {
	Symbol *symbols;
	size count;
	size capacity;
} SymbolTable;

/* Add the given symbol of a type in the current scope if doesn't exists
 * returns:
 * */
void add_symbol(SymbolTable *table, const Symbol symbol);

/* Removes the given symbol of the current scope.
 * returns:
 * */
void remove_symbol(SymbolTable *table, const char *symbol);

/* searches a symbol in the scopes. if it finds it returns the symbol
 * otherwise it returns NULL
 * */
const char *find_symbol(SymbolTable *table, const char *symbol);

/* Creates and returns the main global scope
 * */
SymbolTable *create_table();

void free_symbol_table(SymbolTable *table);

#endif // !SYMBOLTABLE_H
