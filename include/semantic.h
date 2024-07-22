#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symboltable.h"
#include "parser.h"

void semantic_analysis(Node *ast, SymbolTable* tbl);

#endif // !SEMANTIC_H
