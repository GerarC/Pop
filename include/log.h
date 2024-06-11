#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

/* Configuration of the logs */

#define LOG_TAG_SIZE 8
#define LOG_COLOR_SIZE 8
#define PRINT_COLORS 1
#define COLOR_DEBUG "\x1B[34m"
#define COLOR_INFO  "\x1B[32m"
#define COLOR_WARN  "\x1B[33m"
#define COLOR_ERROR "\x1B[31m"
#define COLOR_RESET "\033[0m"

typedef enum LogType {
	LOG_DEBUG = (1 << 0),
	LOG_INFO  = (1 << 1),
	LOG_WARN  = (1 << 2),
	LOG_ERROR = (1 << 4),
} LogType;

void log_debug(const char *message, ...);
void log_info(const char *message, ...);
void log_warn(const char *message, ...);
void log_error(const char *message, ...);

#endif // LOG_H
