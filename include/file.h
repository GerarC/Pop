#ifndef FILE_H
#define FILE_H

#include "config.h"
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

#endif // FILE_H
