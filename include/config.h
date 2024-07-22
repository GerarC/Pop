#ifndef CONFIG_H
#define CONFIG_H

// File Settings
#define MAX_LINE_NUMBER 1000
#define MAX_LINE_SIZE 500
#define MAX_FILE_SIZE MAX_LINE_NUMBER *MAX_LINE_SIZE
#define MAX_FILENAME_SIZE 128

// Token settings
#define MAX_TOK_STRING_SIZE 256

// Symbol Table Settings
#define MAX_SYMBOL_SIZE 64
#define MAX_TABLE_CAPACITY 8338608
#define BUILTIN_TYPE_NUM 8

// IR
#define MAX_STACK_SIZE 1024

// Generation
#define MAX_ASM_LINE_SIZE 1024


#endif // !CONFIG_H

