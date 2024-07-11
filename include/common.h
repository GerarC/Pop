#ifndef COMMMON_H
#define COMMMON_H

#include "config.h"

/* Structure that saves the exact location of a token in the file
 * */
typedef struct _token_location_t {
	int line, col;
	char file[MAX_FILENAME_SIZE];
} SourceLocation;


#endif // !COMMMON_H
