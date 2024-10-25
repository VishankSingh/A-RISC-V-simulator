/**
 * File Name: control_unit.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include <stdint.h>

#include "../include/multi_cycle_vm.h"
#include "../include/control_unit.h"

void update_control_signals(mc_vm_s *vm, uint32_t instruction) {
    uint32_t opcode = instruction & 0x7F;  // Extract opcode (first 7 bits)

    // Reset control signals
    vm->ID_EX.ALUOp = 0;
    vm->ID_EX.ALUSrc = 0;
    vm->ID_EX.Branch = 0;
    vm->ID_EX.MemRead = 0;
    vm->ID_EX.MemWrite = 0;
    vm->ID_EX.MemToReg = 0;
    vm->ID_EX.RegWrite = 0;

    switch (opcode) {
        case 0x33:  // R-type (e.g., add, sub, sll)
            vm->ID_EX.ALUOp = 2;  // ALU operation (e.g., 2 for R-type)
            vm->ID_EX.ALUSrc = 0;  // Use register value as the ALU source
            vm->ID_EX.RegWrite = 1;  // Write to the register file
            break;

        case 0x03:  // I-type Load (e.g., lb, lw)
            vm->ID_EX.ALUOp = 0;  // ALU performs addition for address calculation
            vm->ID_EX.ALUSrc = 1;  // Use immediate value as the ALU source
            vm->ID_EX.MemRead = 1;  // Read from memory
            vm->ID_EX.MemToReg = 1;  // Write the value from memory to register
            vm->ID_EX.RegWrite = 1;  // Write to the register file
            break;

        case 0x13:  // I-type Arithmetic (e.g., addi, slli)
            vm->ID_EX.ALUOp = 0;  // ALU performs addition (or other operation based on funct3)
            vm->ID_EX.ALUSrc = 1;  // Use immediate value as the ALU source
            vm->ID_EX.RegWrite = 1;  // Write to the register file
            break;

        case 0x23:  // S-type Store (e.g., sw, sb)
            vm->ID_EX.ALUOp = 0;  // ALU performs addition for address calculation
            vm->ID_EX.ALUSrc = 1;  // Use immediate value as the ALU source
            vm->ID_EX.MemWrite = 1;  // Write to memory
            break;

        case 0x63:  // B-type Branch (e.g., beq, bne)
            vm->ID_EX.ALUOp = 1;  // ALU performs comparison
            vm->ID_EX.ALUSrc = 0;  // Use register values as the ALU source
            vm->ID_EX.Branch = 1;  // Branch signal active
            break;

        case 0x37:  // U-type LUI (load upper immediate)
            vm->ID_EX.ALUOp = 3;  // Load upper immediate (no actual ALU operation)
            vm->ID_EX.ALUSrc = 1;  // Use immediate value
            vm->ID_EX.RegWrite = 1;  // Write to the register file
            break;

        case 0x6F:  // J-type JAL (jump and link)
            vm->ID_EX.ALUOp = 0;  // ALU doesn't need to perform arithmetic
            vm->ID_EX.ALUSrc = 1;  // Use immediate value as the target address
            vm->ID_EX.RegWrite = 1;  // Write the return address to a register
            vm->ID_EX.Branch = 1;  // Signal branch/jump
            break;

        default:
            // Handle unknown instructions if needed
            break;
    }
}
