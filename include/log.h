#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

/* Configuration of the logs */
#define LOG_TAG_SIZE 8
#define LOG_COLOR_SIZE 8
#define PRINT_FILE 1
#define PRINT_COLORS 1
#define LOG_LEVEL 0 // the most higher level the most log types are printed


typedef enum LogType {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
} LogType;

#define log_trace(...) logger(LOG_TRACE, __FILE__, __VA_ARGS__)
#define log_debug(...) logger(LOG_DEBUG, __FILE__, __VA_ARGS__)
#define log_info(...) logger(LOG_INFO, __FILE__, __VA_ARGS__)
#define log_warn(...) logger(LOG_WARN, __FILE__, __VA_ARGS__)
#define log_error(...) logger(LOG_ERROR, __FILE__, __VA_ARGS__)
#define log_fatal(...) logger(LOG_FATAL, __FILE__, __VA_ARGS__)

void logger(LogType type, const char *file, const char *message, ...);

#endif // LOG_H
