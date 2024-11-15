#include "../include/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void logger(LogType type, const char *file, const int line, const char *message,
			...) {
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
			strcpy(tag, LOG_TEXT_TRACE);
			strcpy(color, COLOR_TRACE);
			output = stdout;
			break;
		case LOG_DEBUG:
			strcpy(tag, LOG_TEXT_DEBUG);
			strcpy(color, COLOR_DEBUG);
			output = stdout;
			break;
		case LOG_INFO:
			strcpy(tag, LOG_TEXT_INFO);
			strcpy(color, COLOR_INFO);
			output = stdout;
			break;
		case LOG_WARN:
			strcpy(tag, LOG_TEXT_WARN);
			output = stdout;
			strcpy(color, COLOR_WARN);
			break;
		case LOG_ERROR:
			strcpy(tag, LOG_TEXT_ERROR);
			strcpy(color, COLOR_ERROR);
			output = stderr;
			break;
		case LOG_FATAL:
			strcpy(tag, LOG_TEXT_FATAL);
			strcpy(color, COLOR_FATAL);
			output = stderr;
			break;
	}

	if (PRINT_COLORS) fprintf(output, LOG_OUTPUT_TAG_COLORED, color, date, tag);
	else fprintf(output,LOG_OUTPUT_TAG, date, tag);

	if (PRINT_FILE) fprintf(output,LOG_OUTPUT_FILE, file);

	if (PRINT_LINE) fprintf(output, LOG_OUTPUT_LINE, line);
	else fprintf(output, LOG_OUTPUT_COLON);

	if (PRINT_COLORS) fprintf(output, LOG_OUTPUT_FINAL_COLOR, COLOR_RESET);

	va_start(ap, message);
	vfprintf(output, message, ap);
	fprintf(output, "\n");
	va_end(ap);
}
