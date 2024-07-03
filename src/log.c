#include "../include/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Colors
#define COLOR_TRACE "\x1B[34m"
#define COLOR_DEBUG "\x1B[36m"
#define COLOR_INFO "\x1B[32m"
#define COLOR_WARN "\x1B[33m"
#define COLOR_ERROR "\x1B[31m"
#define COLOR_FATAL "\x1B[35m"
#define COLOR_RESET "\033[0m"

void logger(LogType type, const char *file, const char *message, ...) {
	if (type < LOG_LEVEL) return;
	va_list ap;
	time_t now;
	time(&now);
	char *date = ctime(&now);
	date[strlen(date) - 6] = '\0';
	char tag[LOG_TAG_SIZE];
	char color[LOG_COLOR_SIZE];
	FILE *output;
	switch (type) {
		case LOG_TRACE:
			strcpy(tag, "TRACE");
			strcpy(color, COLOR_TRACE);
			output = stdout;
			break;
		case LOG_DEBUG:
			strcpy(tag, "DEBUG");
			strcpy(color, COLOR_DEBUG);
			output = stdout;
			break;
		case LOG_INFO:
			strcpy(tag, "INFO ");
			strcpy(color, COLOR_INFO);
			output = stdout;
			break;
		case LOG_WARN:
			strcpy(tag, "WARN ");
			output = stdout;
			strcpy(color, COLOR_WARN);
			break;
		case LOG_ERROR:
			strcpy(tag, "ERROR");
			strcpy(color, COLOR_ERROR);
			output = stderr;
			break;
		case LOG_FATAL:
			strcpy(tag, "FATAL");
			strcpy(color, COLOR_FATAL);
			output = stderr;
			break;
	}

	if (PRINT_COLORS) fprintf(output, "%s%s [%s]", color, date, tag);
	else fprintf(output, "%s [%s] %s: ", date, tag, file);

	if (PRINT_FILE) fprintf(output, " %s: ", file);
	else fprintf(output, ": ");

    if (PRINT_COLORS) fprintf(output, "%s", COLOR_RESET);

	va_start(ap, message);
	vfprintf(output, message, ap);
	fprintf(output, "\n");
	va_end(ap);
}
