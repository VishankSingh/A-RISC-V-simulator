/**
 * File Name: memory.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */


#include "../include/memory.h"
#include "../include/globals.h"

memory_s *init_memory_s(uint64_t size) {
    memory_s *memory = (memory_s *)malloc(sizeof(memory_s));
    MEM_INIT_ERROR(memory, init_memory_s);
    memory->data = (uint8_t *)malloc(size * sizeof(uint8_t));
    MEM_INIT_ERROR(memory->data, init_memory_s);
    memory->change = (uint8_t *)malloc(size * sizeof(uint8_t));
    MEM_INIT_ERROR(memory->change, init_memory_s);
    memory->size = size;
    for (size_t i = 0; i < size; i++) {
        memory->data[i] = 0;
        memory->change[i] = 0;
    }
     memory->data_index = 0;
    return memory;
}

inline void reset_memory(memory_s *memory) {
    for (size_t i = 0; i < memory->size; i++) {
        memory->data[i] = 0;
        memory->change[i] = 0;
    }
    memory->data_index = 0;
}

void write_memory(memory_s *memory, uint64_t address, uint64_t data, int bytes) {
    if (address + bytes - 1 > memory->size) {
        printf("Address out of bounds!\n");
        return;
    }

    // Write the data byte-by-byte in little-endian order
    for (int i = 0; i < bytes; i++) {
        memory->data[address + i] = (data >> (i * 8)) & 0xFF;
        memory->change[address + i] = 1;
    }
}

void write_data_section(memory_s *memory, uint64_t data, size_t data_size) {
    if (memory->data_index + data_size > memory->size) {
        printf("Memory is too small to fit the data section!\n");
        return;
    }

    for (size_t i = 0; i < data_size; i++) {
        memory->data[DATA_MEMORY_START + memory->data_index + i] = (data >> (i * 8)) & 0xFF;
        memory->change[DATA_MEMORY_START + memory->data_index + i] = 1;
    }
    memory->data_index += data_size;

    
}

void load_binary_instructions(memory_s *memory, const uint32_t *binary, size_t instruction_count) {
    uint64_t address = 0x0;  

    if (address + instruction_count * 4 > memory->size) {
        printf("Memory is too small to fit the binary instructions!\n");
        return;
    }

    for (size_t i = 0; i < instruction_count; i++) {
        uint32_t instruction = binary[i];
        for (int j = 0; j < 4; j++) {  
            memory->data[address + j] = (instruction >> (j * 8)) & 0xFF;
            memory->change[address + j] = 1;
        }
        address += 4; 
    }
}

uint64_t read_memory(memory_s *memory, uint64_t address) {
    if (address + 1 > memory->size) {
        printf("Address out of bounds!\n");
        return 0;
    }
    return (uint64_t)memory->data[address];
}

uint64_t read_memory_bytes(memory_s *memory, uint64_t address, int bytes) {
    if (address + bytes > memory->size) {
        printf("Address out of bounds!\n");
        return 0;
    }

    uint64_t value = 0;

    for (int i = 0; i < bytes; i++) {
        value |= ((uint64_t)memory->data[address + i]) << (i * 8);
    }

    return value;
}

uint32_t read_instruction(memory_s *memory, uint64_t address) {
    if (address + 4 > memory->size) {
        printf("Address out of bounds!\n");
        return 0;
    }

    uint32_t value = 0;

    // Read 4 bytes in little-endian order
    for (int i = 0; i < 4; i++) {
        value |= ((uint32_t)memory->data[address + i]) << (i * 8);
    }

    return value;
}

void print_memory(memory_s *memory, uint64_t address, int count) {
    if (address + count > memory->size) {
        printf("Address out of bounds!\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        printf("Memory[0x%05lx] = 0x%X\n", address + i, memory->data[address + i]);
    }
}

void free_memory(memory_s *memory) {
    if (memory) {
        if (memory->data) {
            free(memory->data);
            free(memory->change);
        }
        free(memory);
    }
}
