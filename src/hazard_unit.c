/**
 * File Name: hazard_unit.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "hazard_unit.h"

void hazard_unit(mc_vm_s *vm) {
    find_hazards(vm);
}

void find_hazards(mc_vm_s *vm) {
    if (vm->ID_EX.RegWrite) {
        if (vm->ID_EX.rd == vm->EX_MEM.rs1) {
            //vm->info[vm->ID_EX.pc].get_EXMEM_forward = 1;
        }
        if (vm->ID_EX.rd == vm->EX_MEM.rs2) {
            //vm->info[vm->ID_EX.pc].get_EXMEM_forward = 1;
        }
    }
    if (vm->ID_EX.RegWrite) {
        if (vm->ID_EX.rd == vm->MEM_WB.rs1) {
            //vm->info[vm->ID_EX.pc].get_MEMWB_forward = 1;
        }
        if (vm->ID_EX.rd == vm->MEM_WB.rs2) {
            //vm->info[vm->ID_EX.pc].get_MEMWB_forward = 1;
        }
    }
}

void stall_IF(mc_vm_s *vm) {
    if (vm->is_hazard) {
        vm->is_stalled = 1;
    }
}

void stall_ID(mc_vm_s *vm) {
    if (vm->is_hazard) {
        vm->is_stalled = 1;
    }
}

void stall_EX(mc_vm_s *vm) {
    if (vm->is_hazard) {
        vm->is_stalled = 1;
    }
}

void stall_MEM(mc_vm_s *vm) {
    if (vm->is_hazard) {
        vm->is_stalled = 1;
    }
}

void stall_WB(mc_vm_s *vm) {
    if (vm->is_hazard) {
        vm->is_stalled = 1;
    }
}
