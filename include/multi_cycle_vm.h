/**
 * File Name: multi_cycle_vm.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef MULTI_CYCLE_VM_H
#define MULTI_CYCLE_VM_H

#include "io.h"
#include "memory.h"
#include "registers.h"
#include "vm_input.h"
#include "call_stack.h"
#include "globals.h"

typedef struct instruction_info {
    int instruction_index;
    int stalls;
    _Bool is_executed;

} instruction_info;

typedef struct IF_ID_Register {
    uint32_t instruction;
    uint64_t pc;
    uint64_t npc;
} IF_ID_Register;

typedef struct ID_EX_Register {
    uint64_t pc;
    uint64_t npc;
    uint32_t instruction;
    uint64_t rd;
    uint64_t imm;
    uint64_t rs1_val;
    uint64_t rs2_val;
    int ALUOp;
    int ALUSrc;
    int Branch;
    int MemRead;
    int MemWrite;
    int MemToReg;
    int RegWrite;

} ID_EX_Register;

typedef struct EX_MEM_Register {
    uint64_t pc;
    uint64_t npc;
    uint32_t instruction;
    uint64_t rd;
    uint64_t alu_output;
    uint64_t rs1_val;
    uint64_t rs1;
    uint64_t rs2;
    int Branch;
    int MemRead;
    int MemWrite;
    int MemToReg;
    int RegWrite;
} EX_MEM_Register;

typedef struct MEM_WB_Register {
    uint64_t pc;
    uint64_t npc;
    uint32_t instruction;
    uint64_t rd;
    uint64_t alu_output;
    uint64_t mem_output;
    uint64_t rs1;
    uint64_t rs2;
    int MemToReg;
    int RegWrite;
} MEM_WB_Register;

typedef struct multi_cycle_vm_s {
    char filename[100];
    input_s *input;
    registers_s *register_file;
    memory_s *memory;
    uint64_t *breakpoints;
    uint64_t pc[5];
    IF_ID_Register IF_ID;
    ID_EX_Register ID_EX;
    EX_MEM_Register EX_MEM;
    MEM_WB_Register MEM_WB;
    instruction_info *instruction_info;
    int64_t last_pc[5];
    stack_s *call_stack;
    _Bool is_halted;
    _Bool is_stalled;
    _Bool is_branch;
    _Bool is_jump;
    _Bool is_call;
    _Bool is_ret;
    _Bool is_syscall;
    _Bool is_breakpoint;
    _Bool is_hazard;
    _Bool is_forwarding;
    _Bool is_flushed;
} mc_vm_s;

mc_vm_s *init_multi_cycle_vm_s(char *filename);

void execute_IF(mc_vm_s *vm);

void execute_ID(mc_vm_s *vm);

void execute_EX(mc_vm_s *vm);

void execute_MEM(mc_vm_s *vm);

void execute_WB(mc_vm_s *vm);

void execute(mc_vm_s *vm);

// Instruction Execution Functions
void executeIF_nop(mc_vm_s *vm);
void executeID_nop(mc_vm_s *vm);
void executeEX_nop(mc_vm_s *vm);
void executeMEM_nop(mc_vm_s *vm);
void executeWB_nop(mc_vm_s *vm);

void executeIF_add(mc_vm_s *vm);
void executeID_add(mc_vm_s *vm);
void executeEX_add(mc_vm_s *vm);
void executeMEM_add(mc_vm_s *vm);
void executeWB_add(mc_vm_s *vm);

void executeIF_sub(mc_vm_s *vm);
void executeID_sub(mc_vm_s *vm);
void executeEX_sub(mc_vm_s *vm);
void executeMEM_sub(mc_vm_s *vm);
void executeWB_sub(mc_vm_s *vm);

void executeIF_xor(mc_vm_s *vm);
void executeID_xor(mc_vm_s *vm);
void executeEX_xor(mc_vm_s *vm);
void executeMEM_xor(mc_vm_s *vm);
void executeWB_xor(mc_vm_s *vm);

void executeIF_or(mc_vm_s *vm);
void executeID_or(mc_vm_s *vm);
void executeEX_or(mc_vm_s *vm);
void executeMEM_or(mc_vm_s *vm);
void executeWB_or(mc_vm_s *vm);

void executeIF_and(mc_vm_s *vm);
void executeID_and(mc_vm_s *vm);
void executeEX_and(mc_vm_s *vm);
void executeMEM_and(mc_vm_s *vm);
void executeWB_and(mc_vm_s *vm);

void executeIF_sll(mc_vm_s *vm);
void executeID_sll(mc_vm_s *vm);
void executeEX_sll(mc_vm_s *vm);
void executeMEM_sll(mc_vm_s *vm);
void executeWB_sll(mc_vm_s *vm);

void executeIF_srl(mc_vm_s *vm);
void executeID_srl(mc_vm_s *vm);
void executeEX_srl(mc_vm_s *vm);
void executeMEM_srl(mc_vm_s *vm);
void executeWB_srl(mc_vm_s *vm);

void executeIF_sra(mc_vm_s *vm);
void executeID_sra(mc_vm_s *vm);
void executeEX_sra(mc_vm_s *vm);
void executeMEM_sra(mc_vm_s *vm);
void executeWB_sra(mc_vm_s *vm);

void executeIF_addi(mc_vm_s *vm);
void executeID_addi(mc_vm_s *vm);
void executeEX_addi(mc_vm_s *vm);
void executeMEM_addi(mc_vm_s *vm);
void executeWB_addi(mc_vm_s *vm);

void executeIF_subi(mc_vm_s *vm);
void executeID_subi(mc_vm_s *vm);
void executeEX_subi(mc_vm_s *vm);
void executeMEM_subi(mc_vm_s *vm);
void executeWB_subi(mc_vm_s *vm);

void executeIF_xori(mc_vm_s *vm);
void executeID_xori(mc_vm_s *vm);
void executeEX_xori(mc_vm_s *vm);
void executeMEM_xori(mc_vm_s *vm);
void executeWB_xori(mc_vm_s *vm);

void executeIF_ori(mc_vm_s *vm);
void executeID_ori(mc_vm_s *vm);
void executeEX_ori(mc_vm_s *vm);
void executeMEM_ori(mc_vm_s *vm);
void executeWB_ori(mc_vm_s *vm);

void executeIF_andi(mc_vm_s *vm);
void executeID_andi(mc_vm_s *vm);
void executeEX_andi(mc_vm_s *vm);
void executeMEM_andi(mc_vm_s *vm);
void executeWB_andi(mc_vm_s *vm);

void executeIF_slli(mc_vm_s *vm);
void executeID_slli(mc_vm_s *vm);
void executeEX_slli(mc_vm_s *vm);
void executeMEM_slli(mc_vm_s *vm);
void executeWB_slli(mc_vm_s *vm);

void executeIF_srli(mc_vm_s *vm);
void executeID_srli(mc_vm_s *vm);
void executeEX_srli(mc_vm_s *vm);
void executeMEM_srli(mc_vm_s *vm);
void executeWB_srli(mc_vm_s *vm);

void executeIF_srai(mc_vm_s *vm);
void executeID_srai(mc_vm_s *vm);
void executeEX_srai(mc_vm_s *vm);
void executeMEM_srai(mc_vm_s *vm);
void executeWB_srai(mc_vm_s *vm);

void executeIF_lb(mc_vm_s *vm);
void executeID_lb(mc_vm_s *vm);
void executeEX_lb(mc_vm_s *vm);
void executeMEM_lb(mc_vm_s *vm);
void executeWB_lb(mc_vm_s *vm);

void executeIF_lh(mc_vm_s *vm);
void executeID_lh(mc_vm_s *vm);
void executeEX_lh(mc_vm_s *vm);
void executeMEM_lh(mc_vm_s *vm);
void executeWB_lh(mc_vm_s *vm);

void executeIF_lw(mc_vm_s *vm);
void executeID_lw(mc_vm_s *vm);
void executeEX_lw(mc_vm_s *vm);
void executeMEM_lw(mc_vm_s *vm);
void executeWB_lw(mc_vm_s *vm);

void executeIF_ld(mc_vm_s *vm);
void executeID_ld(mc_vm_s *vm);
void executeEX_ld(mc_vm_s *vm);
void executeMEM_ld(mc_vm_s *vm);
void executeWB_ld(mc_vm_s *vm);

void executeIF_lbu(mc_vm_s *vm);
void executeID_lbu(mc_vm_s *vm);
void executeEX_lbu(mc_vm_s *vm);
void executeMEM_lbu(mc_vm_s *vm);
void executeWB_lbu(mc_vm_s *vm);

void executeIF_lhu(mc_vm_s *vm);
void executeID_lhu(mc_vm_s *vm);
void executeEX_lhu(mc_vm_s *vm);
void executeMEM_lhu(mc_vm_s *vm);
void executeWB_lhu(mc_vm_s *vm);

void executeIF_lwu(mc_vm_s *vm);
void executeID_lwu(mc_vm_s *vm);
void executeEX_lwu(mc_vm_s *vm);
void executeMEM_lwu(mc_vm_s *vm);
void executeWB_lwu(mc_vm_s *vm);

void executeIF_sb(mc_vm_s *vm);
void executeID_sb(mc_vm_s *vm);
void executeEX_sb(mc_vm_s *vm);
void executeMEM_sb(mc_vm_s *vm);
void executeWB_sb(mc_vm_s *vm);

void executeIF_beq(mc_vm_s *vm);
void executeID_beq(mc_vm_s *vm);
void executeEX_beq(mc_vm_s *vm);
void executeMEM_beq(mc_vm_s *vm);
void executeWB_beq(mc_vm_s *vm);

void executeIF_bne(mc_vm_s *vm);
void executeID_bne(mc_vm_s *vm);
void executeEX_bne(mc_vm_s *vm);
void executeMEM_bne(mc_vm_s *vm);
void executeWB_bne(mc_vm_s *vm);

void executeIF_blt(mc_vm_s *vm);
void executeID_blt(mc_vm_s *vm);
void executeEX_blt(mc_vm_s *vm);
void executeMEM_blt(mc_vm_s *vm);
void executeWB_blt(mc_vm_s *vm);

void executeIF_bge(mc_vm_s *vm);
void executeID_bge(mc_vm_s *vm);
void executeEX_bge(mc_vm_s *vm);
void executeMEM_bge(mc_vm_s *vm);
void executeWB_bge(mc_vm_s *vm);

void executeIF_bltu(mc_vm_s *vm);
void executeID_bltu(mc_vm_s *vm);
void executeEX_bltu(mc_vm_s *vm);
void executeMEM_bltu(mc_vm_s *vm);
void executeWB_bltu(mc_vm_s *vm);

void executeIF_bgeu(mc_vm_s *vm);
void executeID_bgeu(mc_vm_s *vm);
void executeEX_bgeu(mc_vm_s *vm);
void executeMEM_bgeu(mc_vm_s *vm);
void executeWB_bgeu(mc_vm_s *vm);

void executeIF_jal(mc_vm_s *vm);
void executeID_jal(mc_vm_s *vm);
void executeEX_jal(mc_vm_s *vm);
void executeMEM_jal(mc_vm_s *vm);
void executeWB_jal(mc_vm_s *vm);

void executeIF_jalr(mc_vm_s *vm);
void executeID_jalr(mc_vm_s *vm);
void executeEX_jalr(mc_vm_s *vm);
void executeMEM_jalr(mc_vm_s *vm);
void executeWB_jalr(mc_vm_s *vm);

void executeIF_lui(mc_vm_s *vm);
void executeID_lui(mc_vm_s *vm);
void executeEX_lui(mc_vm_s *vm);
void executeMEM_lui(mc_vm_s *vm);
void executeWB_lui(mc_vm_s *vm);

void executeIF_auipc(mc_vm_s *vm);
void executeID_auipc(mc_vm_s *vm);
void executeEX_auipc(mc_vm_s *vm);
void executeMEM_auipc(mc_vm_s *vm);
void executeWB_auipc(mc_vm_s *vm);



#endif // MULTI_CYCLE_VM_H