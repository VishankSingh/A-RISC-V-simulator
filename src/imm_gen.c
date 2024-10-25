/**
 * File Name: imm_gen.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "imm_gen.h"

uint64_t imm_gen(uint32_t instruction)
{
    uint64_t imm = 0;
    uint32_t opcode = instruction & 0x7F;
    // uint32_t funct3 = (instruction >> 12) & 0x7;
    // uint32_t funct7 = (instruction >> 25) & 0x7F;
    // uint32_t rd = (instruction >> 7) & 0x1F;
    // uint32_t rs1 = (instruction >> 15) & 0x1F;
    // uint32_t rs2 = (instruction >> 20) & 0x1F;
    uint32_t I_imm = (instruction >> 20);
    uint32_t S_imm = (instruction >> 25) | ((instruction >> 7) & 0x1F);
    uint32_t B_imm = (instruction >> 31) | ((instruction >> 7) & 0x1E) | ((instruction >> 20) & 0x7E) | ((instruction >> 25) & 0x80);
    uint32_t U_imm = (instruction >> 12);
    uint32_t J_imm = (instruction >> 31) | ((instruction >> 12) & 0xFF) | ((instruction >> 20) & 0x7FE) | ((instruction >> 21) & 0x80000);

    switch (opcode)
    {
    case 0x13:
        imm = I_imm;
        break;
    case 0x23:
        imm = S_imm;
        break;
    case 0x63:
        imm = B_imm;
        break;
    case 0x37:
        imm = U_imm;
        break;
    case 0x6F:
        imm = J_imm;
        break;
    default:
        imm = 0;
        break;
    }
    return imm;
}