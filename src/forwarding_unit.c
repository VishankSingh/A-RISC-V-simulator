/**
 * File Name: forwarding_unit.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "forwarding_unit.h"

void forwarding_unit(mc_vm_s *vm) {
    forward_EXMEM(vm);
    forward_MEMWB(vm);
}

void forward_EXMEM(mc_vm_s *vm) {
    if (vm->ID_EX.RegWrite) {
        if (vm->ID_EX.rd == vm->EX_MEM.rs1) {
            vm->ID_EX.rs1_val = vm->EX_MEM.alu_output;
        }
        if (vm->ID_EX.rd == vm->EX_MEM.rs2) {
            vm->ID_EX.rs2_val = vm->EX_MEM.alu_output;
        }
    }
}

void forward_MEMWB(mc_vm_s *vm) {
    if (vm->ID_EX.RegWrite) {
        if (vm->ID_EX.rd == vm->MEM_WB.rs1) {
            vm->ID_EX.rs1_val = vm->MEM_WB.alu_output;
        }
        if (vm->ID_EX.rd == vm->MEM_WB.rs2) {
            vm->ID_EX.rs2_val = vm->MEM_WB.alu_output;
        }
    }
}
