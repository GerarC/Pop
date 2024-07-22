#include "../include/symboltable.h"
#include "../include/log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void print_symbol(Symbol *sym);
void print_table(SymbolTable *table);

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

Symbol create_symbol(Node *node, StructureType stype) {
	Token tok = node->token;
	ScopeType scope = SC_GLOBAL;
	DataType dtype = DT_INT;
	if (strcmp(node->sem_type, "void") == 0) dtype = DT_VOID;
	else if (strcmp(node->sem_type, "int") == 0 || tok.type == TOK_INT)
		dtype = DT_INT;
	else if (strcmp(node->sem_type, "char") == 0 || tok.type == TOK_CHAR == 0)
		dtype = DT_CHAR;
	else {
		log_fatal("%s is not a type: %i", tok.lexeme, tok.type);
		exit(1);
	}
	Symbol sym = {.loc = tok.location,
				  .scope = scope,
				  .dtype = dtype,
				  .stype = stype,
				  .attrs = 1,
				  .offset = 0,
				  .members = NULL};
	strncpy(sym.name, tok.lexeme, MAX_SYMBOL_SIZE);

	return sym;
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

const char *find_symbol_type(SymbolTable *table, int index) {
	DataType dtype = table->symbols[index].dtype;

	switch (dtype) {
		case DT_INT:
			return "int";
			break;

		case DT_FLOAT:
			return "float";
			break;

		case DT_LONG:
			return "long";
			break;

		case DT_CHAR:
			return "char";
			break;

		case DT_VOID:
			return "void";
			break;

		case DT_BOOL:
			return "bool";
			break;

		default:
			log_fatal("%s of not implemented type: %i",
					  table->symbols[index].name, dtype);
			exit(1);
			break;
	}
	return NULL;
}

Symbol get_symbol(SymbolTable *table, int idx) {
	if (table == NULL) {
		log_fatal("Table is null");
		exit(1);
	}
	if (idx > table->count) {
		log_fatal("Out of bounds index %i", idx);
		exit(1);
	}
	return table->symbols[idx];
}

void free_symbol_table(SymbolTable *table) {
	if (table == NULL) return;
	for (int i = 0; i < table->count; i++)
		free_symbol(&table->symbols[i]);
	free(table);
}

void print_symbol(Symbol *sym) {
	printf("%s: %i,%i sc=%i dt=%i st=%i attr=%X ofs=%i", sym->name,
		   sym->loc.line, sym->loc.col, sym->scope, sym->dtype, sym->stype,
		   sym->attrs, sym->offset);
	if (sym->members != NULL) {
		printf(" Members:\n");
		print_table(sym->members);
	} else printf("\n");
}

void print_table(SymbolTable *table) {
	for (int i = 0; i < table->count; i++)
		print_symbol(&table->symbols[i]);
}

void print_symbol_table(SymbolTable *table) {
	if (LOG_DEBUG < LOG_LEVEL) return;
	log_debug("Symbol table");
	print_table(table);
}
