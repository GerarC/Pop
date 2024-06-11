#include "../include/log.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		log_error("You must pass a file to compile");
		printf("USAGE:\n"
			   "\t./pop -c <file> -> To compile the program\n"
			   "\t./pop -s <file> -> To create only assembler code\n");
		return 0;
	}
	if (argc == 3)
		log_debug("%s %s", argv[1], argv[2]);
	if (strcmp(argv[1], "-c") == 0) {
		log_debug("hola");
	}
	log_debug("Pop compiler");

	return 0;
}
