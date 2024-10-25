/**
 * File Name: constants.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_MAIN_TOKEN_LIST_SIZE 1000
#define MAX_LABEL_TABLE_SIZE 1000
#define MAX_TOKEN_NAME_SIZE 100
#define LEXER_LINE_BUFFER 1024
#define MAX_PARSER_TEMP_TOKEN_LINE 20

#define MAX_LABEL_SIZE 30
#define MAX_LABELS 100

#define MAX_INPUT_FILE_SIZE 1000
#define MAX_MEMORY_SIZE 0x50000
#define DATA_MEMORY_START 0x10000
#define MAX_BREAKPOINTS 6
#define CALL_STACK_SIZE 1000

#define MAX_LOOP_ITERATIONS 1000

#define MEM_INIT_ERROR(ptr, func_name) \
        do { \
            if (ptr == NULL) { \
                perror(#func_name); \
                return NULL; \
            } \
        } while (0)

extern char *output_file_;
extern _Bool error_code_;

#endif // CONSTANTS_H