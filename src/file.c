#include "../include/file.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>

void file_error(const char *message, const char *file) {
	log_fatal("'%s' %s", file);
	exit(1);
}

char *read_file(const char *source) {
	log_info("Reading %s", source);

	char *buffer = 0;
	long lenght;

	FILE *code_file = fopen(source, "rb");

	if (!code_file) file_error("file doesn't exists", source);

	fseek(code_file, 0, SEEK_END);
	lenght = ftell(code_file);
	fseek(code_file, 0, SEEK_SET);
	buffer = (char *)malloc(sizeof(char) * (lenght + 1));
	if (buffer) fread(buffer, sizeof(char), lenght, code_file);
	fclose(code_file);
	buffer[lenght] = '\0';

	return buffer;
}
