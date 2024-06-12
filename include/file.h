#ifndef FILE_H
#define FILE_H

#define MAX_LINE_NUMBER 1000
#define MAX_LINE_SIZE 500
#define MAX_FILE_SIZE MAX_LINE_NUMBER *MAX_LINE_SIZE

/* Initializes an array of MAX_LINE_NUMBER * MAX_LINE_SIZE
 * This will contain all the lines of the program.
 * */
char **initialize_lines();

/* Reads the file from the given source. If the file exists
 * will storage all the code lines that exist in.
 *
 * Input:
 *  source: path of the file.
 *  code_lines: array that will contain code_lines (should be initialize with
 * initialize_lines function).
 *  line_counter: will count how many lines of code are inside that file.
 * */
void read_file(const char *source, char **code_lines, int *line_counter);

/* Given a destination path and an array of code, this will write that code into
 * the file.
 * */
void write_file(const char *destination, char **code, int length);

/* Frees an array that has been initialized with initialize_lines
 * */
void free_lines(char **lines);

#endif // FILE_H
