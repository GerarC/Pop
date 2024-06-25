#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define MAX_TYPE_SIZE 128
#define MAX_SYMBOL_SIZE 64
#define MAX_ENTRY_SCOPES 256
#define MAX_ENTRY_SYMBOLS 256
#define MAX_TABLE_ENTRIES 128

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
	int sym_num;
} TableEntry;

/* Symbol Table has a list of Entries. It means that has a list of types and the
 * symbols they belong
 * */
typedef struct _symbol_table {
	TableEntry entries[MAX_ENTRY_SCOPES];
	int entry_num;
} SymbolTable;

/* Manages symbols and types that are allowed in a determined scope block
 * (function, loop, if...). Each scope has a link to its parent
 * */
typedef struct _scope_t {
	SymbolTable table;
	struct _scope_t *parent;
} Scope;

void add_entry(Scope *scope, const char *type);

void delete_entry(Scope *scope, const char *type);

void add_symbol(Scope *scope, const char *symbol);

void remove_symbol(Scope *scope, const char *symbol);

char *find_symbol(Scope *scope, const char *symbol);

#endif // !SYMBOLTABLE_H
