#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "./config.h"
#include "./typedef.h"
/* It's a wrapper structure to represent a symbol in the entry table
 * */
typedef struct _symbol_t {
	char name[MAX_SYMBOL_SIZE];
} Symbol;

/* The entry of the SymbolTable. Each entry saves the type and the list of items
 * of that type.
 * */
typedef struct _table_entry_t {
	char type[MAX_TYPE_SIZE];
	Symbol symbols[MAX_ENTRY_SYMBOLS];
	size count;
} TableEntry;

/* Symbol Table has a list of Entries. It means that has a list of types and the
 * symbols they belong
 * */
typedef struct _symbol_table {
	TableEntry entries[MAX_ENTRY_SCOPES];
	size count;
} SymbolTable;

/* Manages symbols and types that are allowed in a determined scope block
 * (function, loop, if...). Each scope has a link to its parent
 * */
typedef struct _scope_t {
	SymbolTable table;
	struct _scope_t *parent;
} Scope;

/* Adds a new type entry into the table of the current scope
 * returns:
 * -  -1 if the type already exists
 * -  -2 if the table has been reached the limit of types
 * - idx the index of the new element
 * */
int add_entry(Scope *scope, const char *type);

int delete_entry(Scope *scope, const char *type);

/* Add the given symbol of a type in the current scope if doesn't exists
 * returns:
 *  - -1 if the type doesn't exists in any scope
 *  - -2 if the symbol already exists in the scope
 *  - -3 if the entry scope has been reached the limit of symbols
 *  -  0 if all goes well
 * */
int add_symbol(Scope *scope, const char *type, const char *symbol);

/* Removes the given symbol of the current scope.
 * returns:
 * -  0 if all goes well
 * - -1 if the symbol doesn't exists in the current scope
 * */
int remove_symbol(Scope *scope, const char *symbol);

/* searches a symbol in the scopes. if it finds it returns the symbol
 * otherwise it returns NULL
 * */
const char *find_symbol(Scope *scope, const char *symbol);

/* Creates and returns the main global scope
 * */
Scope *create_global_scope();

/* Creates a return a scope. receives the parent scope
 * */
Scope *enter_scope(Scope *parent);

/* Deletes a global scope and returns its father
 * */
Scope *exit_scope(Scope *scope);

#endif // !SYMBOLTABLE_H
