/**
 * File Name: registers.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define uint unsigned int

typedef struct Registers {
    int64_t *reg_arr;
    uint64_t last_changed;
} registers_s;

registers_s *init_registers_s();

void reset_registers(registers_s *registers);

void write_registers(registers_s *registers, uint register_no, uint64_t value);

int64_t read_registers(registers_s *registers, uint register_no);

void print_registers(registers_s *registers);

void free_registers(registers_s *registers);

#endif // REGISTERS_H