#include "../include/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void logger(LogType type, const char *message, va_list args) {
	time_t now;
	time(&now);
	char *date = ctime(&now);
	date[strlen(date) - 1] = '\0';
	char tag[LOG_TAG_SIZE];
	char color[LOG_COLOR_SIZE];
	FILE *output;
	switch (type) {
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
	}
	if (PRINT_COLORS)
		fprintf(output, "%s%s [%s]:%s ", color, date, tag, COLOR_RESET);
	else
		fprintf(output, "%s [%s]: ", date, tag);
	vfprintf(output, message, args);
	fprintf(output, "\n");
}

void log_debug(const char *message, ...) {
	va_list args;
	va_start(args, message);
	logger(LOG_DEBUG, message, args);
	va_end(args);
}

void log_info(const char *message, ...) {
	va_list args;
	va_start(args, message);
	logger(LOG_INFO, message, args);
	va_end(args);
}

void log_warn(const char *message, ...) {
	va_list args;
	va_start(args, message);
	logger(LOG_WARN, message, args);
	va_end(args);
}

void log_error(const char *message, ...) {
	va_list args;
	va_start(args, message);
	logger(LOG_ERROR, message, args);
	va_end(args);
}
