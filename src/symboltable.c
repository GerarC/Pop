#include "../include/symboltable.h"
#include "../include/log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int add_symbol(Scope *scope, const char *type, const char *symbol) {
	int type_idx = 0;
	Scope *current = scope;
	TableEntry curr_entry;
	// Find the type
	while (current != NULL) {
		for (type_idx = 0; type_idx < current->table.count; type_idx++) {
			curr_entry = current->table.entries[type_idx];
			// Outs of the loop if type is found

			if (strcmp(curr_entry.type, type) == 0) goto out_add_loop;
		}
		current = current->parent;
	}

	// As all scopes have been iterated and no one owns the type,
	// so it doesn't exist.
	log_fatal("The type doesn't exist");
	return -1;
out_add_loop:

	// Check if the symbol exists in the current scope
	for (int i = 0; i < scope->table.count; i++)
		for (int j = 0; j < scope->table.entries[i].count; j++)
			if (strcmp(scope->table.entries[i].symbols[j].name, symbol) == 0)
				return -2;

	// Type exists but the current scope doesn't have any symbol of it
	if (scope != current) type_idx = add_entry(scope, type);
	if (type_idx == -2) {
		log_error("Entry is full");
		return -4;
	}

	// Add the symbol to the table
	int symbol_num = scope->table.entries[type_idx].count;
	if (symbol_num < MAX_ENTRY_SYMBOLS) {
		// type_idx must be the correct because if the scope doesn't have it,
		// add_entry returns it, in the other hand it's already set by the loop
		strncpy(scope->table.entries[type_idx].symbols[symbol_num].name, symbol,
				MAX_SYMBOL_SIZE);
		assert(strcmp(scope->table.entries[type_idx].symbols[symbol_num].name,
					  symbol) == 0 &&
			   "ERROR: types are different");
		scope->table.entries[type_idx].count++;
		return 0;
	} else return -3;
}

int remove_symbol(Scope *scope, const char *symbol) {
	int type_idx, sym_idx;
	int found = 0;
	for (type_idx = 0; type_idx < scope->table.count; type_idx++)
		for (int sym_idx = 0; sym_idx < scope->table.entries[type_idx].count;
			 sym_idx++)
			if (strcmp(scope->table.entries[type_idx].symbols[sym_idx].name,
					   symbol) == 0) {
				found = 1;
				goto out_remove_loops;
			}

out_remove_loops:
	if (!found) return -1;
	for (int i = sym_idx; i < scope->table.entries[type_idx].count - 1; i++) {
		scope->table.entries[type_idx].symbols[i] =
			scope->table.entries[type_idx].symbols[i + 1];
	}
	scope->table.entries[type_idx].count--;
	return 0;
}

int add_entry(Scope *scope, const char *type) {
	int count = scope->table.count;
	for (int i = 0; i < count; i++)
		if (strcmp(scope->table.entries[i].type, type) == 0) return -1;

	if (count >= MAX_TABLE_ENTRIES) return -2;
	strncpy(scope->table.entries[count].type, type, MAX_TYPE_SIZE);
	scope->table.entries[count].count = 0;
	scope->table.count++;
	return count;
}

int delete_entry(Scope *scope, const char *type) {
	int type_idx = 0;
	int count = scope->table.count;
	int found = 0;
	for (type_idx = 0; type_idx < count; type_idx++)
		if (strcmp(scope->table.entries[type_idx].type, type) == 0) {
			found = 1;
			goto out_delete_entry_loop;
		}

out_delete_entry_loop:
	if (!found) return -1;
	for (int i = type_idx; i < scope->table.count - 1; i++)
		scope->table.entries[i] = scope->table.entries[i + 1];
	scope->table.count--;
	return 0;
}

const char *find_symbol(Scope *scope, const char *symbol) {
	Scope *current = scope;
	SymbolTable curr_table;
	TableEntry curr_entry;
	int symbol_num;
	while (current != NULL) {
		curr_table = current->table;
		for (int i = 0; i < current->table.count; i++) {
			curr_entry = curr_table.entries[i];
			symbol_num = curr_entry.count;
			for (int j = 0; j < symbol_num; j++) {
				Symbol sym = curr_entry.symbols[j];
				if (strcmp(symbol, sym.name) == 0)
					return current->table.entries[i].type;
			}
		}
		current = current->parent;
	}
	return NULL;
}

Scope *create_global_scope() {
	Scope *scope = (Scope *)malloc(sizeof(Scope));
	if (scope == NULL) return NULL;
	scope->table.count = 0;
	const char *builtin_types[BUILTIN_TYPE_NUM] = {
		"object", "int", "float", "char", "bool", "string", "byte", "hex"};
	for (int i = 0; i < BUILTIN_TYPE_NUM; i++)
		add_entry(scope, builtin_types[i]);
	return scope;
}

Scope *enter_scope(Scope *parent) {
	Scope *scope = (Scope *)malloc(sizeof(Scope));
	if (scope == NULL) return NULL;
	scope->table.count = 0;
	scope->parent = parent;

	return scope;
}

Scope *exit_scope(Scope *scope) {
	Scope *parent = scope->parent;
	free(scope);
	return parent;
}
