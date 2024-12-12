/**
 * File Name: memory.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Memory {
    uint8_t *data;
    uint8_t *change;
    size_t size;
    size_t data_index;
} memory_s;

memory_s *init_memory_s(size_t memory_size);

void reset_memory(memory_s *memory);

void write_memory(memory_s *memory, uint64_t address, uint64_t data, int bytes);

void write_data_section(memory_s *memory, uint64_t data, size_t data_size);

void load_binary_instructions(memory_s *memory, const uint32_t *binary, size_t instruction_count);

uint64_t read_memory(memory_s *memory, uint64_t address);

uint64_t read_memory_bytes(memory_s *memory, uint64_t address, int bytes);

uint32_t read_instruction(memory_s *memory, uint64_t address);

void print_memory(memory_s *memory, uint64_t address, int count);

void free_memory(memory_s *memory);

#endif // MEMORY_H