/**
 * File Name: lexer.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef LEXER_H
#define LEXER_H

#include "../include/tokens.h"

#define uint unsigned int

typedef struct Lexer {
    token_list_s *token_list;
    char *filename;
    char *line;
    uint line_number;
    uint instruction_number;
    uint character_number;
} lexer_s;

lexer_s *init_lexer(token_list_s *token_list, char *filename);

void lex(lexer_s *lexer);

_Bool tokenize_line(lexer_s *lexer);

void free_lexer(lexer_s *lexer);

#endif // LEXER_H