#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>

/* Configuration of the logs */
#define LOG_TAG_SIZE 8
#define LOG_COLOR_SIZE 8
#define PRINT_FILE 1
#define PRINT_LINE 1
#define PRINT_COLORS 1
#define LOG_LEVEL 0 // the most lower level the most log types are printed

// Colors
#define COLOR_TRACE "\x1B[34m"
#define COLOR_DEBUG "\x1B[36m"
#define COLOR_INFO "\x1B[32m"
#define COLOR_WARN "\x1B[33m"
#define COLOR_ERROR "\x1B[31m"
#define COLOR_FATAL "\x1B[35m"
#define COLOR_RESET "\033[0m"

// Constants
#define LOG_TEXT_TRACE "TRACE"
#define LOG_TEXT_DEBUG "DEBUG"
#define LOG_TEXT_INFO  "INFO "
#define LOG_TEXT_WARN  "WARN "
#define LOG_TEXT_ERROR "ERROR"
#define LOG_TEXT_FATAL "FATAL"
#define LOG_TEXT_RESET "RESET"

#define LOG_OUTPUT_TAG_COLORED  "%s%s [%s]"
#define LOG_OUTPUT_TAG  "%s [%s]"
#define LOG_OUTPUT_FILE " %s:"
#define LOG_OUTPUT_LINE "%u: "
#define LOG_OUTPUT_COLON ": "
#define LOG_OUTPUT_FINAL_COLOR "%s"


typedef enum LogType {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
} LogType;

#define log_trace(...) logger(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) logger(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) logger(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) logger(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) logger(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) logger(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void logger(LogType type, const char *file, const int ln, const char *msg, ...);

#endif // LOG_H
