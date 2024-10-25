/**
 * File Name: instructions.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "tokens.h"
#include "vm_input.h"

#define uint unsigned int

typedef struct R_Type {
    uint32_t funct7: 7;
    uint32_t funct3: 3;
    uint32_t opcode: 7;
} R_Type;

typedef struct I_Type {
    uint32_t funct3: 3;
    uint32_t opcode: 7;
} I_Type;

typedef struct S_Type {
    uint32_t funct3: 3;
    uint32_t opcode: 7;
} S_Type;

typedef struct B_Type {
    uint32_t funct3: 3;
    uint32_t opcode: 7;
} B_Type;

typedef struct U_Type {
    uint32_t opcode: 7;
} U_Type;

typedef struct J_Type {
    uint32_t opcode: 7;
} J_Type;

// R-Type Instructions //
extern R_Type add_instruction;
extern R_Type sub_instruction;
extern R_Type xor_instruction;
extern R_Type or_instruction;
extern R_Type and_instruction;
extern R_Type sll_instruction;
extern R_Type srl_instruction;
extern R_Type sra_instruction;
extern R_Type slt_instruction;
extern R_Type sltu_instruction;

// I-Type Instructions //
extern I_Type addi_instruction;
extern I_Type xori_instruction;
extern I_Type ori_instruction;
extern I_Type andi_instruction;
extern I_Type slli_instruction;
extern I_Type srli_instruction;
extern I_Type srai_instruction;
extern I_Type slti_instruction;
extern I_Type sltiu_instruction;
extern I_Type lb_instruction;
extern I_Type lh_instruction;
extern I_Type lw_instruction;
extern I_Type ld_instruction;
extern I_Type lbu_instruction;
extern I_Type lhu_instruction;
extern I_Type lwu_instruction;
extern I_Type jalr_instruction;
extern I_Type ecall_instruction;
extern I_Type ebreak_instruction;

// S-Type Instructions //
extern S_Type sb_instruction;
extern S_Type sh_instruction;
extern S_Type sw_instruction;
extern S_Type sd_instruction;

// B-Type Instructions //
extern B_Type beq_instruction;
extern B_Type bne_instruction;
extern B_Type blt_instruction;
extern B_Type bge_instruction;
extern B_Type bltu_instruction;
extern B_Type bgeu_instruction;

// U-Type Instructions //
extern U_Type lui_instruction;
extern U_Type auipc_instruction;

// J-Type Instructions //
extern J_Type jal_instruction;

extern int r_format[4];
extern int i_format[4];
extern int s_format[6];
extern int b_format[4];
extern int u_format[3];
extern int j_format[3];

char check_instruction_type(char *instruction);

_Bool valid_R_format(token_list_s *instruction_tokens);

_Bool valid_I_bracket_format(token_list_s *instruction_tokens);

_Bool valid_I_format(token_list_s *instruction_tokens);

_Bool valid_S_format(token_list_s *instruction_tokens);

_Bool valid_B_imm_format(token_list_s *instruction_tokens);

_Bool valid_B_label_format(token_list_s *instruction_tokens);

_Bool valid_U_format(token_list_s *instruction_tokens);

_Bool valid_J_imm_format(token_list_s *instruction_tokens);

_Bool valid_J_label_format(token_list_s *instruction_tokens);

uint32_t R_format_binary_code(R_Type instruction, uint rd, uint rs1, uint rs2);

uint32_t I_format_binary_code(I_Type instruction, uint rd, uint rs1, int imm);

uint32_t S_format_binary_code(S_Type instruction, uint rs1, uint rs2, uint imm);

uint32_t B_format_binary_code(B_Type instruction, uint rs1, uint rs2, int imm);

uint32_t U_format_binary_code(U_Type instruction, uint rd, int imm);

uint32_t J_format_binary_code(J_Type instruction, uint rd, int imm);

void generate_bit_string(char *code, uint rd, uint rs1, uint rs2, int imm, input_s *input);


#endif // INSTRUCTIONS_H