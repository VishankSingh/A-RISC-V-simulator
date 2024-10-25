/**
 * File Name: tokens.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define uint unsigned int

typedef struct Token {
    enum {
        TOKEN_INSTRUCTION,
        TOKEN_REGISTER,
        TOKEN_IMMEDIATE,
        TOKEN_LABEL,
        TOKEN_LABEL_REF,
        TOKEN_L_PAREN,
        TOKEN_R_PAREN,
        TOKEN_SECTION,
        TOKEN_D_DIRECTIVE
    } token_type;
    uint line_number;
    uint character_number;
    uint instruction_number;
    char *value;
} token_s;

typedef struct TokenList {
    uint index;
    uint size;
    token_s *list;
} token_list_s;

token_s *init_token(int token_type, uint line_number, uint character_number, uint instruction_number, char *value);

void print_token(token_s *token);

token_list_s *init_token_list_s(uint size);

void print_token_list(token_list_s *token_list);

void free_token_list(token_list_s *token_list);

char *get_register_value(char *register_name);

#endif // TOKENS_H