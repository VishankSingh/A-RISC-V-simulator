/**
 * File Name: vm.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef VM_H
#define VM_H

#include "io.h"
#include "memory.h"
#include "registers.h"
#include "vm_input.h"
#include "stack.h"

struct VM;

typedef void (*execute_func_t)(uint32_t, struct VM *);

typedef struct lookup_table_s {
    execute_func_t opcode_table[10];
    execute_func_t funct6_table[10][8][2];
    execute_func_t funct3_table[10][8];
    execute_func_t funct7_table[10][8][3];
} lookup_table_s;

typedef struct VM {
    char filename[100];
    input_s *input;
    registers_s *register_file;
    memory_s *memory;
    lookup_table_s *lookup_table;
    uint64_t *breakpoints;
    uint64_t pc;
    int64_t last_pc;
    stack_s *call_stack;
} vm_s;

vm_s *init_vm_s(char *filename);

void vm_set_filename(vm_s *vm, char *filename);

void vm_reset(vm_s *vm, char* filename);

void add_breakpoint(vm_s *vm, uint64_t address);

void remove_breakpoint(vm_s *vm, uint64_t address);

_Bool in_breakpoint(vm_s *vm, uint64_t address);

void vm_run(vm_s *vm, _Bool run);

void vm_execute_instruction(vm_s *vm, uint32_t instruction);

void free_vm_s(vm_s *vm);

lookup_table_s *init_lookup_table();

void free_lookup_table(lookup_table_s *lookup_table);

void execute_add(uint32_t instruction, vm_s *vm);
void execute_sub(uint32_t instruction, vm_s *vm);
void execute_xor(uint32_t instruction, vm_s *vm);
void execute_or(uint32_t instruction, vm_s *vm);
void execute_and(uint32_t instruction, vm_s *vm);
void execute_sll(uint32_t instruction, vm_s *vm);
void execute_srl(uint32_t instruction, vm_s *vm);
void execute_sra(uint32_t instruction, vm_s *vm);

void execute_addi(uint32_t instruction, vm_s *vm);
void execute_subi(uint32_t instruction, vm_s *vm);
void execute_xori(uint32_t instruction, vm_s *vm);
void execute_ori(uint32_t instruction, vm_s *vm);
void execute_andi(uint32_t instruction, vm_s *vm);
void execute_slli(uint32_t instruction, vm_s *vm);
void execute_srli(uint32_t instruction, vm_s *vm);
void execute_srai(uint32_t instruction, vm_s *vm);

void execute_lb(uint32_t instruction, vm_s *vm);
void execute_lh(uint32_t instruction, vm_s *vm);
void execute_lw(uint32_t instruction, vm_s *vm);
void execute_ld(uint32_t instruction, vm_s *vm);
void execute_lbu(uint32_t instruction, vm_s *vm);
void execute_lhu(uint32_t instruction, vm_s *vm);
void execute_lwu(uint32_t instruction, vm_s *vm);

void execute_sb(uint32_t instruction, vm_s *vm);
void execute_sh(uint32_t instruction, vm_s *vm);
void execute_sw(uint32_t instruction, vm_s *vm);
void execute_sd(uint32_t instruction, vm_s *vm);

void execute_beq(uint32_t instruction, vm_s *vm);
void execute_bne(uint32_t instruction, vm_s *vm);
void execute_blt(uint32_t instruction, vm_s *vm);
void execute_bge(uint32_t instruction, vm_s *vm);
void execute_bltu(uint32_t instruction, vm_s *vm);
void execute_bgeu(uint32_t instruction, vm_s *vm);

void execute_jal(uint32_t instruction, vm_s *vm);
void execute_jalr(uint32_t instruction, vm_s *vm);

void execute_lui(uint32_t instruction, vm_s *vm);
void execute_auipc(uint32_t instruction, vm_s *vm);

void execute_ecall(uint32_t instruction, vm_s *vm);

#endif // VM_H