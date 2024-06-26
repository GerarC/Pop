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
#define MAX_TYPE_SIZE 128
#define MAX_SYMBOL_SIZE 64
#define MAX_ENTRY_SCOPES 256
#define MAX_ENTRY_SYMBOLS 256
#define MAX_TABLE_ENTRIES 128
#define BUILTIN_TYPE_NUM 8

// Generation
#define MAX_ASM_LINE_SIZE 128

#endif // !CONFIG_H

