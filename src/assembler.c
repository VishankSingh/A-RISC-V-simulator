/**
 * File Name: assembler.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */


#include "../include/assembler.h"
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/globals.h"



void assemble(char *output_file, vm_s *vm) {
    create_file(output_file);
    token_list_s *token_list = init_token_list_s(MAX_MAIN_TOKEN_LIST_SIZE);
    lexer_s *lexer = init_lexer(token_list, vm->filename);
    if (lexer == NULL) {
        return;
    }
    lex(lexer);
    // print_token_list(token_list);
    free_lexer(lexer);
    parser_s *parser = init_parser(token_list, vm);
    if (parser == NULL) {
        return;
    }
    parse(parser);
    free_parser(parser);
    // print_li_map(vm->input->li_map);
    //free_token_list(token_list);
    if (error_code_) {
        remove(output_file);
        
    }
}