/**
 * File Name: utils.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>


#define uint unsigned int

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"
#define UNDL  "\x1B[4m"
#define BOLD_OFF "\e[m"

void int32_to_binary(uint32_t value, char *binary_str);

uint decimal_to_binary(int decimal, int bit_length, _Bool MSB_extend);

void print_binary(uint32_t number);

_Bool is_valid_decimal(const char *str);

_Bool is_valid_hexadecimal(const char *str);

_Bool is_valid_binary(const char *str);

_Bool is_unsigned_instruction(const char *str);

_Bool is_funct6_instruction(const char *instruction);

_Bool is_I_Bracket_instruction(const char *instruction);

void print_error(char *filename, uint line_number, uint character_number, const char *line_1, const char *line_2);

void print_dash_n_times(int n);

void print_space_n_times(uint n);

void print_32_bit_instruction(uint num);

int is_regular_file(const char *path);

#endif // UTILS_H