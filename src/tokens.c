/**
 * File Name: tokens.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */


#include "../include/tokens.h"
#include "../include/globals.h"

#define uint unsigned int

token_s *init_token(int token_type,
                         uint line_number,
                         uint character_number,
                         uint instruction_number,
                         char *value) {
    token_s *token = (token_s *)malloc(sizeof(token_s));
    MEM_INIT_ERROR(token, init_token);
    token->token_type = token_type;
    token->line_number = line_number;
    token->character_number = character_number;
    token->instruction_number = instruction_number;
    token->value = value;
    return token;
}

token_list_s *init_token_list_s(uint size) {
    token_list_s *token_list = (token_list_s *)malloc(sizeof(token_list_s));
    MEM_INIT_ERROR(token_list, init_token_list_s);
    token_list->index = 0;
    token_list->size = 0;
    token_list->list = (token_s *)malloc(size * sizeof(token_s));
    MEM_INIT_ERROR(token_list->list, init_token_list_s);
    return token_list;
}

void print_token(token_s *token) {
    switch (token->token_type) {
        case TOKEN_SECTION:
            printf("<%d:%d:TOKEN_SECTION:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_D_DIRECTIVE:
            printf("<%d:%d:TOKEN_D_DIRECTIVE:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_INSTRUCTION:
            printf("<%d:%d:TOKEN_INSTRUCTION:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_REGISTER:
            printf("<%d:%d:TOKEN_REGISTER:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_IMMEDIATE:
            printf("<%d:%d:TOKEN_IMMEDIATE:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_LABEL:
            printf("<%d:%d:TOKEN_LABEL:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_LABEL_REF:
            printf("<%d:%d:TOKEN_LABEL_REF:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_L_PAREN:
            printf("<%d:%d:TOKEN_L_PAREN:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        case TOKEN_R_PAREN:
            printf("<%d:%d:TOKEN_R_PAREN:%s>\n",
                   token->line_number,
                   token->character_number,
                   token->value);
            break;
        default:printf("[-] Error: Unknown token type\n");
            break;
    }
}

void print_token_list(token_list_s *token_list) {
    size_t i = 0;
    while (i < token_list->index) {
        uint j = token_list->list[i].instruction_number;
        while (j == token_list->list[i].instruction_number) {
            print_token(&token_list->list[i]);
            i++;
        }
        printf("\n");
    }
}

void free_token_list(token_list_s *token_list) {
    if (token_list == NULL) {
        return;
    }
    if (token_list->list == NULL) {
        free(token_list);
        return;
    }
    if (token_list->list != NULL) {
        for (uint i = 0; i < token_list->index; i++) {
            if (token_list->list[i].value != NULL) {
                free(token_list->list[i].value);
            }
        }
        free(token_list->list);
    }
    free(token_list);
}


// Temporary

char *get_register_value(char *register_name) {
    if ((strcmp(register_name, "x0") == 0) || (strcmp(register_name, "zero") == 0)) {
        return "0";
    } else if ((strcmp(register_name, "x1") == 0) || (strcmp(register_name, "ra") == 0)) {
        return "1";
    } else if ((strcmp(register_name, "x2") == 0) || (strcmp(register_name, "sp") == 0)) {
        return "2";
    } else if ((strcmp(register_name, "x3") == 0) || (strcmp(register_name, "gp") == 0)) {
        return "3";
    } else if ((strcmp(register_name, "x4") == 0) || (strcmp(register_name, "tp") == 0)) {
        return "4";
    } else if ((strcmp(register_name, "x5") == 0) || (strcmp(register_name, "t0") == 0)) {
        return "5";
    } else if ((strcmp(register_name, "x6") == 0) || (strcmp(register_name, "t1") == 0)) {
        return "6";
    } else if ((strcmp(register_name, "x7") == 0) || (strcmp(register_name, "t2") == 0)) {
        return "7";
    } else if ((strcmp(register_name, "x8") == 0) || (strcmp(register_name, "s0") == 0)
        || (strcmp(register_name, "fp") == 0)) {
        return "8";
    } else if ((strcmp(register_name, "x9") == 0) || (strcmp(register_name, "s1") == 0)) {
        return "9";
    } else if ((strcmp(register_name, "x10") == 0) || (strcmp(register_name, "a0") == 0)) {
        return "10";
    } else if ((strcmp(register_name, "x11") == 0) || (strcmp(register_name, "a1") == 0)) {
        return "11";
    } else if ((strcmp(register_name, "x12") == 0) || (strcmp(register_name, "a2") == 0)) {
        return "12";
    } else if ((strcmp(register_name, "x13") == 0) || (strcmp(register_name, "a3") == 0)) {
        return "13";
    } else if ((strcmp(register_name, "x14") == 0) || (strcmp(register_name, "a4") == 0)) {
        return "14";
    } else if ((strcmp(register_name, "x15") == 0) || (strcmp(register_name, "a5") == 0)) {
        return "15";
    } else if ((strcmp(register_name, "x16") == 0) || (strcmp(register_name, "a6") == 0)) {
        return "16";
    } else if ((strcmp(register_name, "x17") == 0) || (strcmp(register_name, "a7") == 0)) {
        return "17";
    } else if ((strcmp(register_name, "x18") == 0) || (strcmp(register_name, "s2") == 0)) {
        return "18";
    } else if ((strcmp(register_name, "x19") == 0) || (strcmp(register_name, "s3") == 0)) {
        return "19";
    } else if ((strcmp(register_name, "x20") == 0) || (strcmp(register_name, "s4") == 0)) {
        return "20";
    } else if ((strcmp(register_name, "x21") == 0) || (strcmp(register_name, "s5") == 0)) {
        return "21";
    } else if ((strcmp(register_name, "x22") == 0) || (strcmp(register_name, "s6") == 0)) {
        return "22";
    } else if ((strcmp(register_name, "x23") == 0) || (strcmp(register_name, "s7") == 0)) {
        return "23";
    } else if ((strcmp(register_name, "x24") == 0) || (strcmp(register_name, "s8") == 0)) {
        return "24";
    } else if ((strcmp(register_name, "x25") == 0) || (strcmp(register_name, "s9") == 0)) {
        return "25";
    } else if ((strcmp(register_name, "x26") == 0) || (strcmp(register_name, "s10") == 0)) {
        return "26";
    } else if ((strcmp(register_name, "x27") == 0) || (strcmp(register_name, "s11") == 0)) {
        return "27";
    } else if ((strcmp(register_name, "x28") == 0) || (strcmp(register_name, "t3") == 0)) {
        return "28";
    } else if ((strcmp(register_name, "x29") == 0) || (strcmp(register_name, "t4") == 0)) {
        return "29";
    } else if ((strcmp(register_name, "x30") == 0) || (strcmp(register_name, "t5") == 0)) {
        return "30";
    } else if ((strcmp(register_name, "x31") == 0) || (strcmp(register_name, "t6") == 0)) {
        return "31";
    } else {
        return NULL;
    }
}