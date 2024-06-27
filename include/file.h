#ifndef FILE_H
#define FILE_H

#include "./config.h"
/* Reads the file from the given source. If the file exists
 * will storage all the code lines that exist in.
 *
 * Input:
 *  source: path of the file.
 * */
char* read_file(const char *source);

/* Given a destination path and an array of code, this will write that code into
 * the file.
 * */
void write_file(const char *destination, char **code, int length);

/* Frees an array that has been initialized with initialize_lines
 * */
void free_lines(char *lines);

#endif // FILE_H
