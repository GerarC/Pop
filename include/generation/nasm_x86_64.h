
#ifndef NASM_X86_64_H
#define NASM_X86_64_H

#include "../ir.h"

void generate_nasm_x86_64(const char* destination, IntermediateRepresentation *ir, SymbolTable *tbl);


#endif // !NASM_X86_64_H
