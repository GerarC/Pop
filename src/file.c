#include "../include/file.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *source) {
	log_info("Reading %s", source);

	char *buffer = 0;
	long lenght;

	FILE *code_file = fopen(source, "rb");

	if (!code_file) {
		log_fatal("file called '%s' doesn't exists", source);
		exit(1);
	}

	fseek(code_file, 0, SEEK_END);
	lenght = ftell(code_file);
	fseek(code_file, 0, SEEK_SET);
	buffer = (char *)malloc(sizeof(char) * (lenght + 1));
	if (buffer) fread(buffer, sizeof(char), lenght, code_file);
	fclose(code_file);
	buffer[lenght] = '\0';

	return buffer;
}

void write_file(const char *destination, Assembler *code) {
	FILE *destination_f = fopen(destination, "w");
	for (int i = 0; i < code->count; i++)
		fputs(code->lines[i], destination_f);
	fclose(destination_f);
}
