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
	FILE *code_file = fopen(source, "r");
	char *line;
	ssize_t lenght = 0;
	size_t buffer_size = 0;
	*line_counter = 0;
	while ((lenght = getline((char **)&line, &buffer_size, code_file)) >= 0) {
		strlcpy(code_lines[*line_counter], line, MAX_LINE_SIZE);
		log_debug("line -> %s", line);
		log_debug("code_line -> %s", code_lines[*line_counter]);
	}
	fclose(code_file);
	free(line);
}

void write_file(const char *destination, char **code, int line_counter) {
	FILE *code_file = fopen(destination, "w");
	for (int i = 0; i < line_counter; i++){
		fputs(code[line_counter], code_file);
		fputs("\n", code_file);
    }
}

void free_lines(char **lines) {
	for (int i = 0; i < MAX_LINE_NUMBER; i++)
		free(lines[i]);
	free(lines);
}
