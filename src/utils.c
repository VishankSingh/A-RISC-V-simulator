/**
 * File Name: utils.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "../include/utils.h"
#include "../include/io.h"

void int32_to_binary(uint32_t value, char *binary_str) {
    for (int i = 31; i >= 0; i--) {
        binary_str[31 - i] = (value & (1 << i)) ? '1' : '0';
    }
    binary_str[32] = '\0';  // Null-terminate the string
}

void print_binary(uint32_t number) {
    for (int i = 31; i >= 0; i--) {
        uint32_t bit = (number >> i) & 1;
        printf("%u", bit);
        if (i == 7 || i == 12 || i == 15 || i == 20 || i == 25)
            putchar(' ');
    }
}

_Bool is_valid_decimal(const char *str) {
    uint i = 0;
    if (str[0] == '-' || str[0] == '+')
        i = 1;
    while (str[i] != '\0') {
        if (!isdigit(str[i]))
            return 0;
        i++;
    }
    return 1;
}

_Bool is_valid_hexadecimal(const char *str) {
    uint i = 0;
    if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
        i += 2;
    else
        return 0;

    while (str[i] != '\0') {
        if (!isxdigit(str[i]))
            return 0;
        i++;
    }
    return 1;
}

_Bool is_valid_binary(const char *str) {
    uint i = 0;

    if (str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B')) {
        i += 2;
    } else {
        return 0;
    }

    while (str[i] != '\0') {
        if (str[i] != '0' && str[i] != '1') {
            return 0;
        }
        i++;
    }
    return 1;
}

_Bool is_unsigned_instruction(const char *instruction) {
    if (strcmp(instruction, "lbu") == 0 || strcmp(instruction, "lhu") == 0 || strcmp(instruction, "lwu") == 0
        || strcmp(instruction, "sltiu") == 0 || strcmp(instruction, "lui") == 0 || strcmp(instruction, "auipc") == 0) {
        return 1;
    }
    return 0;
}

_Bool is_funct6_instruction(const char *instruction) {
    if (strcmp(instruction, "slli") == 0
        || strcmp(instruction, "srli") == 0
        || strcmp(instruction, "srai") == 0)
        return 1;
    return 0;
}

_Bool is_I_Bracket_instruction(const char *instruction) {
    _Bool is = ((strcmp(instruction, "jalr") == 0)
                || (strcmp(instruction, "lb") == 0)
                || (strcmp(instruction, "lh") == 0)
                || (strcmp(instruction, "lw") == 0)
                || (strcmp(instruction, "ld") == 0)
                || (strcmp(instruction, "lbu") == 0)
                || (strcmp(instruction, "lhu") == 0)
                || (strcmp(instruction, "lwu") == 0));
    return is;
}

void print_error(char *filename, uint line_number, uint character_number, const char *msg, const char *line_2) {
    char* line = read_line(filename, line_number);
    
    printf(BOLD "%s:%d:%d: " RED "[error]" RESET " %s\n",
           filename,
           line_number,
           character_number,
           msg);
    printf("%5d | %s\n", line_number, line);
    printf("      | %s\n", line_2);
    printf("      |\n");
    free(line);
}

void print_dash_n_times(int n) {
    for (int i = 0; i < n; i++)
        putchar('-');
}

void print_space_n_times(uint n) {
    for (uint i = 0; i < n; i++)
        putchar(' ');
}

void print_32_bit_instruction(uint num) {
    for (int i = 31; i >= 0; i--) {
        putchar((num & (1 << i)) ? '1' : '0');
        if (i == 7 || i == 12 || i == 15 || i == 20 || i == 25)
            putchar(' ');
    }
    putchar('\n');
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}