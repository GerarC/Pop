#ifndef FILE_H
#define FILE_H

#define MAX_LINE_NUMBER 1000
#define MAX_LINE_SIZE 500
#define MAX_FILE_SIZE MAX_LINE_NUMBER *MAX_LINE_SIZE

char **initialize_lines();
void read_file(const char *source, char **code_lines, int *line_counter);
void write_file(const char *destination, char **code, int line_counter);
void free_lines(char **lines);

#endif // FILE_H
