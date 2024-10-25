/**
 * File Name: io.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef IO_H
#define IO_H

#include <stdint.h>

#define uint unsigned int

void create_file(char *filename);

void write_newline_to_file(char *filename, uint32_t machine_code);

char *read_line(char *filename, uint line_number);

int count_lines_in_file(const char *filename);

#endif // IO_H