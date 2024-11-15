#ifndef MAIN_CONSTANTS_H
#define MAIN_CONSTANTS_H

#define OPTION_COMPILE "-c"
#define OPTION_ASM "-s"
#define OPTION_OUTPUT "-o"
#define OPTION_DEBUG "--debug"

#define POP_COMPILER "Pop compiler"

#define USAGE_MESSAGE                                                          \
	"cli error.\n"                                                             \
	"USAGE:\n"                                                                 \
	"\t%s " OPTION_COMPILE " <file> -> To compile the program\n"               \
	"\t%s " OPTION_ASM "<file> -> To create only assembler code\n"             \
	"\t%s " OPTION_OUTPUT " <file> -> Output file\n"                           \
	"\t%s " OPTION_DEBUG " -> Active debug mode\n"

//ERROR
#define ERROR_BAD_ARGUMENT_MESSAGE "Bad argument '%s'"

#endif // !MAIN_CONSTANTS_H
