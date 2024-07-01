#ifndef FILE_H
#define FILE_H

#include "config.h"
#include "asmlines.h"
/* Reads the file from the given source. If the file exists
 * will storage all the code lines that exist in.
 *
 * Input:
 *  source: path of the file.
 *
 * Returns:
 *  code (char): the readed code. must be freed
 * */
char* read_file(const char *source);

/* Given a destination path and an array of code, this will write that code into
 * the file.
 * */
void write_file(const char *destination, Assembler *code);

#endif // FILE_H
