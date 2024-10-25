/**
 * File Name: registers.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */


#include "../include/registers.h"
#include "../include/globals.h"

#define uint unsigned int

registers_s *init_registers_s() {
    registers_s *registers = (registers_s *)malloc(sizeof(registers_s));
    MEM_INIT_ERROR(registers, init_registers_s);
    registers->reg_arr = (int64_t *)malloc(32 * sizeof(uint64_t));
    MEM_INIT_ERROR(registers->reg_arr, init_registers_s);
    for(int i = 0; i < 32; i++) {
        registers->reg_arr[i] = 0;
    }
    registers->last_changed = 0;
    return registers;
}

inline void reset_registers(registers_s *registers) {
    for(int i = 0; i < 32; i++) {
        registers->reg_arr[i] = 0;
    }
    registers->last_changed = 0;
}

void write_registers(registers_s *registers, uint register_no, uint64_t value) {
    if (register_no >= 32) {
        perror("write_registers");
        return;
    }
    if (register_no == 0) return;
    registers->reg_arr[register_no] = value;
    registers->last_changed = register_no;
}

int64_t read_registers(registers_s *registers, uint register_no) {
    if (register_no >= 32) {
        perror("read_registers");
        return 0;
    }
    return registers->reg_arr[register_no];
}

void print_registers(registers_s *registers) {
    printf("Registers: \n");
    for(int i = 0; i < 32; i++) {
        printf("x%-2d = 0x%lX\n", i, registers->reg_arr[i]);
    }
}

void free_registers(registers_s *registers) {
    if (registers) {
        if (registers->reg_arr) {
            free(registers->reg_arr);
        }
        free(registers);
    }
}