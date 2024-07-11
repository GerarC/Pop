#include "../include/symboltable.h"
#include "../include/log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void add_symbol(SymbolTable *table, const Symbol symbol) {
	if (table->count >= table->capacity) {
		table->capacity *= 2;
		table->symbols =
			(Symbol *)realloc(table->symbols, sizeof(Symbol) * table->capacity);
	}
	strncpy(table->symbols[table->count].name, symbol.name, MAX_SYMBOL_SIZE);
	table->symbols[table->count].loc = symbol.loc;
	table->symbols[table->count].scope = symbol.scope;
	table->symbols[table->count].dtype = symbol.dtype;
	table->symbols[table->count].stype = symbol.stype;
	table->symbols[table->count].attrs = symbol.attrs;
	table->symbols[table->count].offset = symbol.offset;
	table->symbols[table->count].members = symbol.members;
	table->count++;
}

void remove_symbol(SymbolTable *table, const char *symbol) {
	log_trace("remove symbol not implemented");
}

int find_symbol(SymbolTable *table, const char *symbol) {
	for (int i = 0; i < table->count; i++) {
		if (strcmp(table->symbols[i].name, symbol) == 0) return i;
	}
	return -1;
}

/* Creates and returns the main global scope
 * */
SymbolTable *create_table() {
	SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
	table->symbols = (Symbol *)malloc(sizeof(Symbol));
	table->capacity = 1;
	table->count = 0;
	return table;
}

void free_symbol(Symbol *sym) {
	if (sym == NULL) return;
	if (sym->members != NULL) free_symbol_table(sym->members);
}

void free_symbol_table(SymbolTable *table) {
	if (table == NULL) return;
	for (int i = 0; i < table->count; i++)
		free_symbol(&table->symbols[i]);
	free(table);
}
