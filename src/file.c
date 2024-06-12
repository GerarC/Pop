#include "../include/file.h"
#include "../include/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **initialize_lines() {
	char **lines = (char **)malloc(sizeof(char *) * MAX_LINE_NUMBER);
	for (int i = 0; i < MAX_LINE_NUMBER; i++)
		lines[i] = malloc(sizeof(char) * MAX_LINE_SIZE);
	return lines;
}

void read_file(const char *source, char **code_lines, int *line_counter) {
	log_info("Leyendo %s", source);
	FILE *code_file = fopen(source, "r");
	if (!code_file) {
		log_error("El archivo %s no existe");
		return;
	}
	char *line = NULL;
	ssize_t lenght = 0;
	size_t buffer_size = 0;
	*line_counter = 0;
	while ((lenght = getline(&line, &buffer_size, code_file)) >= 0) {
		strlcpy(code_lines[*line_counter], line, MAX_LINE_SIZE);
		log_debug("code_line -> %s", code_lines[*line_counter]);
        *line_counter += 1;
	}
	fclose(code_file);
	free(line);
}

void write_file(const char *destination, char **code, int length) {
	FILE *code_file = fopen(destination, "w");
	for (int i = 0; i < length; i++) {
		fputs(code[length], code_file);
		fputs("\n", code_file);
	}
}

void free_lines(char **lines) {
	for (int i = 0; i < MAX_LINE_NUMBER; i++)
		free(lines[i]);
	free(lines);
}
