#ifndef FILE_H
#define FILE_H

#include "config.h"
/* Reads the file from given source. If file exists
 * will storage into a char* the whole content.
 *
 * Input:
 * - source: path of the file.
 *
 * Returns:
 * - code (char): the readed code. must be freed
 * */
char *read_file(const char *source);

#endif // FILE_H
