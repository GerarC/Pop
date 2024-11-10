#include "../include/file.h"
#include "../include/constant/file_constants.h"
#include "../include/constant/common_constants.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>

void file_error(const char *message, const char *file) {
	log_fatal(FILE_ERROR_MESSAGE_TEMPLATE, file);
	exit(1);
}

char *read_file(const char *source) {
	log_info(FILE_READING_MESSAGE, source);

	char *buffer = 0;
	long lenght;

	FILE *code_file = fopen(source, "rb");

	if (!code_file) file_error(FILE_DOESNT_EXISTS_MESSAGE, source);

	fseek(code_file, 0, SEEK_END);
	lenght = ftell(code_file);
	fseek(code_file, 0, SEEK_SET);
	buffer = (char *)malloc(sizeof(char) * (lenght + 1));
	if (buffer) fread(buffer, sizeof(char), lenght, code_file);
	fclose(code_file);
	buffer[lenght] = FINAL_CHAR;

	return buffer;
}
