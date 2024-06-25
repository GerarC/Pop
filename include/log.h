#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

/* Configuration of the logs */
#define LOG_TAG_SIZE 8
#define LOG_COLOR_SIZE 8
#define PRINT_COLORS 1
#define LOG_LEVEL 5 // the most higher level the most log types are printed

// Colors
#define COLOR_TRACE "\x1B[34m"
#define COLOR_DEBUG "\x1B[36m"
#define COLOR_INFO "\x1B[32m"
#define COLOR_WARN "\x1B[33m"
#define COLOR_ERROR "\x1B[31m"
#define COLOR_FATAL "\x1B[35m"
#define COLOR_RESET "\033[0m"

typedef enum LogType {
	LOG_FATAL,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
	LOG_TRACE,
} LogType;

void log_trace(const char *message, ...);
void log_debug(const char *message, ...);
void log_info(const char *message, ...);
void log_warn(const char *message, ...);
void log_error(const char *message, ...);
void log_fatal(const char *message, ...);

#endif // LOG_H
