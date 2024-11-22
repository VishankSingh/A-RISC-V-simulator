/**
 * File Name: lexer.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "../include/instructions.h"
#include "../include/lexer.h"
#include "../include/globals.h"


lexer_s *init_lexer(token_list_s *token_list, char *filename) {
    lexer_s *lexer = (lexer_s *)malloc(sizeof(lexer_s));
    MEM_INIT_ERROR(lexer, init_lexer);
    lexer->token_list = token_list;
    lexer->filename = filename;
    lexer->line = NULL;
    lexer->line_number = 0;
    lexer->character_number = 0;
    lexer->instruction_number = 0;
    return lexer;
}

_Bool tokenize_line(lexer_s *lexer) {
    size_t i = 0;
    _Bool is_text_section = 1;
    unsigned int initial_size = lexer->token_list->size;
    while (i < strlen(lexer->line)) {
        lexer->character_number = i + 1;
        if ((lexer->line[i] == ' ') || (lexer->line[i] == '\t') || (lexer->line[i] == ',')) {
            i++;
            continue;
        } else if (lexer->line[i] == '#' || lexer->line[i] == ';') {
            break;
        } else if (lexer->line[i] == '(') {
            token_s *token =
                init_token(TOKEN_L_PAREN, lexer->line_number, lexer->character_number, lexer->instruction_number, "(");
            lexer->token_list->list[lexer->token_list->index++] = *token;
            lexer->token_list->size++;
            free(token);
            i++;
            continue;
        } else if (lexer->line[i] == ')') {
            token_s *token =
                init_token(TOKEN_R_PAREN, lexer->line_number, lexer->character_number, lexer->instruction_number, ")");
            lexer->token_list->list[lexer->token_list->index++] = *token;
            lexer->token_list->size++;
            free(token);
            i++;
            continue;
        } else {
            char *token_name = (char *)malloc(MAX_TOKEN_NAME_SIZE * sizeof(char));
            MEM_INIT_ERROR(token_name, tokenize_line);
            int j = 0;
            while (lexer->line[i] != ' ' && lexer->line[i] != '\t' && lexer->line[i] != ',' && lexer->line[i] != '('
                && lexer->line[i] != ')' && lexer->line[i] != '\n' && lexer->line[i] != '\0' && lexer->line[i] != '#'
                ) {
                token_name[j] = lexer->line[i];
                i++;
                j++;
            }
            token_name[j] = '\0';
            if (strcmp(token_name, ".text") == 0 || strcmp(token_name, ".data") == 0) {
                if (strcmp(token_name, ".text") == 0) {
                    is_text_section = 1;
                } else  {
                    is_text_section = 0;
                }
                token_s
                    *token =
                    init_token(TOKEN_SECTION,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (strcmp(token_name, ".dword") == 0 || strcmp(token_name, ".word") == 0
                    || strcmp(token_name, ".half") == 0 || strcmp(token_name, ".byte") == 0) {
                token_s
                    *token =
                    init_token(TOKEN_D_DIRECTIVE,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if ((j > 1) && (token_name[j - 1] == ':') && (!isdigit(token_name[0]))) {
                token_name[j - 1] = '\0';
                token_s
                    *token =
                    init_token(TOKEN_LABEL,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (get_register_value(token_name) != NULL) {
                token_s
                    *token =
                    init_token(TOKEN_REGISTER,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               get_register_value(token_name));
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (is_valid_binary(token_name) || is_valid_decimal(token_name) || is_valid_hexadecimal(token_name)) {
                long long decimal_value;
                if (is_valid_binary(token_name)) {
                    token_name += 2;
                    decimal_value = strtoull(token_name, NULL, 2);
                    sprintf(token_name, "%lld", decimal_value);
                } else if (is_valid_hexadecimal(token_name)) {
                    token_name += 2;
                    decimal_value = strtoull(token_name, NULL, 16);
                    sprintf(token_name, "%lld", decimal_value);
                }
                token_s
                    *token =
                    init_token(TOKEN_IMMEDIATE,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (check_instruction_type(token_name) != 'O') {
                token_s
                    *token =
                    init_token(TOKEN_INSTRUCTION,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            } else if (lexer->token_list->index > 0 && isalpha(token_name[0])
                ) {
                token_s
                    *token =
                    init_token(TOKEN_LABEL_REF,
                               lexer->line_number,
                               lexer->character_number,
                               lexer->instruction_number,
                               token_name);
                lexer->token_list->list[lexer->token_list->index++] = *token;
                lexer->token_list->size++;
                free(token);
                continue;
            }
            free(token_name);
        }
        printf(BOLD "%s:%d:%d: " RED "[error]" RESET " Unexpected token\n",
               lexer->filename,
               lexer->line_number,
               lexer->character_number);
        printf("%5d | %s\n", lexer->line_number, lexer->line);
        printf("      |");
        print_space_n_times(lexer->character_number);
        printf(RED "^\n" RESET);

        error_code_ = 1;
        break;
    }

    _Bool if_instruction = 0;

    for (unsigned int j = initial_size; j < lexer->token_list->index; j++) {
        if (lexer->token_list->list[j].token_type == TOKEN_INSTRUCTION) {
            if_instruction = 1;
            break;
        }
    }

    if (!if_instruction || !is_text_section) {
        lexer->instruction_number--;
        return error_code_;
    }

    

    return error_code_;
}

void lex(lexer_s *lexer) {
    lexer->line = (char *)malloc(LEXER_LINE_BUFFER * sizeof(char));
    FILE *input_file = fopen(lexer->filename, "r");
    if (!lexer->line) {
        printf(RED "[error]" RESET "%s:1: failed to allocate memory for line buffer\n", lexer->filename);
        fclose(input_file);
        free(lexer);
        return;
    }
    
    while (fgets(lexer->line, LEXER_LINE_BUFFER, input_file) != NULL) {
        lexer->line_number++;

        size_t length = strlen(lexer->line);
        if (length > 0 && lexer->line[length - 1] == '\n') {
            lexer->line[length - 1] = '\0';
            length--;
        }

        size_t i = 0;
        while (i < length && (lexer->line[i] == ' ' || lexer->line[i] == '\t')) {
            i++;
        }
        if (i == length) {
            continue;
        }

        if (lexer->line[i] == '#' || lexer->line[i] == ';') {
            continue;
        }

        lexer->instruction_number++;
        tokenize_line(lexer);
    }

    fclose(input_file);

    free(lexer->line);
}

void free_lexer(lexer_s *lexer) {
    free(lexer);
}
