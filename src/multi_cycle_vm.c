/**
 * File Name: multi_cycle_vm.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "../include/multi_cycle_vm.h"
#include "../include/imm_gen.h"
#include "../include/forwarding_unit.h"
#include "../include/control_unit.h"

mc_vm_s *multi_cycle_vm_s(char *filename) {
    mc_vm_s *vm = (mc_vm_s *)malloc(sizeof(mc_vm_s));
    strcpy(vm->filename, filename);
    vm->input = init_input_s(MAX_INPUT_FILE_SIZE, vm->filename);
    vm->register_file = init_registers_s();
    vm->memory = init_memory_s(MAX_MEMORY_SIZE);
    vm->breakpoints = NULL;
    vm->pc[0] = 1;
    vm->pc[1] = 0;
    vm->pc[2] = 0;
    vm->pc[3] = 0;
    vm->pc[4] = 0;
    vm->last_pc[0] = 0;
    vm->last_pc[1] = 0;
    vm->last_pc[2] = 0;
    vm->last_pc[3] = 0;
    vm->last_pc[4] = 0;
    vm->instruction_info = (instruction_info *)malloc(MAX_INPUT_FILE_SIZE * sizeof(instruction_info));
    vm->call_stack = init_stack_s(CALL_STACK_SIZE);
    return vm;
}

void execute(mc_vm_s *vm) {
    forward_EXMEM(vm);
    forward_MEMWB(vm);
    execute_WB(vm);
    execute_MEM(vm);
    execute_EX(vm);
    execute_ID(vm);
    execute_IF(vm);
}

void execute_IF(mc_vm_s *vm) {
    if (vm->is_flushed) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        vm->is_flushed = 0;
        return;
    }
    if (vm->is_stalled) {
        return;
    }
    if (vm->is_halted) {
        return;
    }
    if (vm->is_branch) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_jump) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_call) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_ret) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_syscall) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_breakpoint) {
        vm->IF_ID.instruction = 0;
        vm->IF_ID.pc = 0;
        vm->IF_ID.npc = 0;
        return;
    }
    if (vm->is_hazard) {
        return;
    }
    if (vm->is_forwarding) {
        return;
    }
    vm->IF_ID.instruction = read_instruction(vm->memory, vm->pc[0]);
    vm->IF_ID.pc = vm->pc[0];
    vm->IF_ID.npc = vm->pc[0] + 1;

    if (vm->IF_ID.instruction == 0) {
        vm->is_halted = 1;
    }

    uint64_t opcode = vm->IF_ID.instruction & 0x7F;
    if (opcode == 0x6F || opcode == 0x67 || opcode == 0x63) {
        vm->is_branch = 1;
    }
    if (opcode == 0x6 || opcode == 0x7) {
        vm->is_jump = 1;
    }
    if (opcode == 0x73) {
        vm->is_call = 1;
    }
    if (opcode == 0x6F) {
        vm->is_ret = 1;
    }
    if (opcode == 0x73) {
        vm->is_syscall = 1;
    }
    if (opcode == 0x0F) {
        vm->is_breakpoint = 1;
    }

    vm->pc[0]++;
}

void execute_ID(mc_vm_s *vm) {
    if (vm->is_flushed) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_stalled) {
        return;
    }
    if (vm->is_halted) {
        return;
    }
    if (vm->is_branch) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_jump) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_call) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_ret) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_syscall) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_breakpoint) {
        vm->ID_EX.instruction = 0;
        vm->ID_EX.pc = 0;
        vm->ID_EX.npc = 0;
        return;
    }
    if (vm->is_hazard) {
        return;
    }
    if (vm->is_forwarding) {
        return;
    }
    vm->ID_EX.instruction = vm->IF_ID.instruction;
    vm->ID_EX.pc = vm->IF_ID.pc;
    vm->ID_EX.npc = vm->IF_ID.npc;

    uint64_t opcode = vm->ID_EX.instruction & 0x7F;
    if (opcode == 0x6F || opcode == 0x67 || opcode == 0x63) {
        vm->is_branch = 1;
    }
    if (opcode == 0x6 || opcode == 0x7) {
        vm->is_jump = 1;
    }
    if (opcode == 0x73) {
        vm->is_call = 1;
    }
    if (opcode == 0x6F) {
        vm->is_ret = 1;
    }
    if (opcode == 0x73) {
        vm->is_syscall = 1;
    }
    if (opcode == 0x0F) {
        vm->is_breakpoint = 1;
    }
    uint64_t rd = (vm->ID_EX.instruction >> 7) & 0b11111;
    uint64_t imm = imm_gen(vm->ID_EX.instruction);
    uint64_t rs1_val = read_registers(vm->register_file, (vm->ID_EX.instruction >> 15) & 0b11111);
    uint64_t rs2_val = read_registers(vm->register_file, (vm->ID_EX.instruction >> 20) & 0b11111);

    vm->ID_EX.rd = rd;
    vm->ID_EX.imm = imm;
    vm->ID_EX.rs1_val = rs1_val;
    vm->ID_EX.rs2_val = rs2_val;
    update_control_signals(vm, vm->ID_EX.instruction);
}

void execute_EX(mc_vm_s *vm) {
    if (vm->is_flushed) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_stalled) {
        return;
    }
    if (vm->is_halted) {
        return;
    }
    if (vm->is_branch) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_jump) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_call) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_ret) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_syscall) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_breakpoint) {
        vm->EX_MEM.instruction = 0;
        vm->EX_MEM.pc = 0;
        vm->EX_MEM.npc = 0;
        return;
    }
    if (vm->is_hazard) {
        return;
    }
    if (vm->is_forwarding) {
        return;
    }
    vm->EX_MEM.instruction = vm->ID_EX.instruction;
    vm->EX_MEM.pc = vm->ID_EX.pc;
    vm->EX_MEM.npc = vm->ID_EX.npc;
    vm->EX_MEM.rd = vm->ID_EX.rd;
}

void execute_MEM(mc_vm_s *vm) {
    if (vm->is_flushed) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_stalled) {
        return;
    }
    if (vm->is_halted) {
        return;
    }
    if (vm->is_branch) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_jump) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_call) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_ret) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_syscall) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_breakpoint) {
        vm->MEM_WB.instruction = 0;
        vm->MEM_WB.pc = 0;
        vm->MEM_WB.npc = 0;
        return;
    }
    if (vm->is_hazard) {
        return;
    }
    if (vm->is_forwarding) {
        return;
    }
    vm->MEM_WB.instruction = vm->EX_MEM.instruction;
    vm->MEM_WB.pc = vm->EX_MEM.pc;
    vm->MEM_WB.npc = vm->EX_MEM.npc;
    vm->MEM_WB.rd = vm->EX_MEM.rd;
    vm->MEM_WB.alu_output = vm->EX_MEM.alu_output;
    vm->MEM_WB.mem_output = vm->memory->data[vm->EX_MEM.alu_output];
}

void execute_WB(mc_vm_s *vm) {
    if (vm->is_flushed) {
        return;
    }
    if (vm->is_stalled) {
        return;
    }
    if (vm->is_halted) {
        return;
    }
    if (vm->is_branch) {
        return;
    }
    if (vm->is_jump) {
        return;
    }
    if (vm->is_call) {
        return;
    }
    if (vm->is_ret) {
        return;
    }
    if (vm->is_syscall) {
        return;
    }
    if (vm->is_breakpoint) {
        return;
    }
    if (vm->is_hazard) {
        return;
    }
    if (vm->is_forwarding) {
        return;
    }
    vm->last_pc[4] = vm->MEM_WB.pc;
    vm->pc[4] = vm->MEM_WB.npc;
    vm->instruction_info[vm->last_pc[4]].is_executed = 1;
}
