/**
 * File Name: main.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "../include/instructions.h"
#include "../include/globals.h"
#include "../include/io.h"


#define uint unsigned int

R_Type add_instruction = {0b0000000, 0b000, 0b0110011};
R_Type sub_instruction = {0b0100000, 0b000, 0b0110011};
R_Type xor_instruction = {0b0000000, 0b100, 0b0110011};
R_Type or_instruction = {0b0000000, 0b110, 0b0110011};
R_Type and_instruction = {0b0000000, 0b111, 0b0110011};
R_Type sll_instruction = {0b0000000, 0b001, 0b0110011};
R_Type srl_instruction = {0b0000000, 0b101, 0b0110011};
R_Type sra_instruction = {0b0100000, 0b101, 0b0110011};
R_Type slt_instruction = {0b0000000, 0b010, 0b0110011};
R_Type sltu_instruction = {0b0000000, 0b011, 0b0110011};

I_Type addi_instruction = {0b000, 0b0010011};
I_Type xori_instruction = {0b100, 0b0010011};
I_Type ori_instruction = {0b110, 0b0010011};
I_Type andi_instruction = {0b111, 0b0010011};
I_Type slli_instruction = {0b001, 0b0010011};
I_Type srli_instruction = {0b101, 0b0010011};
I_Type srai_instruction = {0b101, 0b0010011};
I_Type slti_instruction = {0b010, 0b0010011};
I_Type sltiu_instruction = {0b011, 0b0010011};

I_Type lb_instruction = {0b000, 0b0000011};
I_Type lh_instruction = {0b001, 0b0000011};
I_Type lw_instruction = {0b010, 0b0000011};
I_Type ld_instruction = {0b011, 0b0000011};
I_Type lbu_instruction = {0b100, 0b0000011};
I_Type lhu_instruction = {0b101, 0b0000011};
I_Type lwu_instruction = {0b110, 0b0000011};

I_Type jalr_instruction = {0b000, 0b1100111};

I_Type ecall_instruction = {0b000, 0b1110011};

S_Type sb_instruction = {0b000, 0b0100011};
S_Type sh_instruction = {0b001, 0b0100011};
S_Type sw_instruction = {0b010, 0b0100011};
S_Type sd_instruction = {0b011, 0b0100011};

B_Type beq_instruction = {0b000, 0b1100011};
B_Type bne_instruction = {0b001, 0b1100011};
B_Type blt_instruction = {0b100, 0b1100011};
B_Type bge_instruction = {0b101, 0b1100011};
B_Type bltu_instruction = {0b110, 0b1100011};
B_Type bgeu_instruction = {0b111, 0b1100011};

U_Type lui_instruction = {0b0110111};
U_Type auipc_instruction = {0b0010111};

J_Type jal_instruction = {0b1101111};

char check_instruction_type(char *instruction) {
    if (strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0
        || strcmp(instruction, "xor") == 0 || strcmp(instruction, "or") == 0
        || strcmp(instruction, "and") == 0 || strcmp(instruction, "sll") == 0
        || strcmp(instruction, "srl") == 0 || strcmp(instruction, "sra") == 0
        || strcmp(instruction, "slt") == 0 || strcmp(instruction, "sltu") == 0) {
        return 'R';
    } else if (strcmp(instruction, "addi") == 0 || strcmp(instruction, "xori") == 0
               || strcmp(instruction, "ori") == 0 || strcmp(instruction, "andi") == 0
               || strcmp(instruction, "slli") == 0 || strcmp(instruction, "srli") == 0
               || strcmp(instruction, "srai") == 0 || strcmp(instruction, "slti") == 0
               || strcmp(instruction, "sltiu") == 0 || strcmp(instruction, "jalr") == 0
               || strcmp(instruction, "lb") == 0 || strcmp(instruction, "lh") == 0
               || strcmp(instruction, "lw") == 0 || strcmp(instruction, "ld") == 0
               || strcmp(instruction, "lbu") == 0 || strcmp(instruction, "lhu") == 0
               || strcmp(instruction, "lwu") == 0 || strcmp(instruction, "ecall") == 0) {
        return 'I';
    } else if (strcmp(instruction, "sb") == 0 || strcmp(instruction, "sh") == 0
               || strcmp(instruction, "sw") == 0 || strcmp(instruction, "sd") == 0) {
        return 'S';
    } else if (strcmp(instruction, "beq") == 0 || strcmp(instruction, "bne") == 0
               || strcmp(instruction, "blt") == 0 || strcmp(instruction, "bge") == 0
               || strcmp(instruction, "bltu") == 0 || strcmp(instruction, "bgeu") == 0) {
        return 'B';
    } else if (strcmp(instruction, "lui") == 0 || strcmp(instruction, "auipc") == 0) {
        return 'U';
    } else if (strcmp(instruction, "jal") == 0) {
        return 'J';
    } else if (strcmp(instruction, "nop") == 0 || strcmp(instruction, "li") == 0
               || strcmp(instruction, "mv") == 0 || strcmp(instruction, "not") == 0
               || strcmp(instruction, "neg") == 0 || strcmp(instruction, "seqz") == 0
               || strcmp(instruction, "snez") == 0 || strcmp(instruction, "sltz") == 0
               || strcmp(instruction, "sgtz") == 0 || strcmp(instruction, "beqz") == 0
               || strcmp(instruction, "bnez") == 0 || strcmp(instruction, "blez") == 0
               || strcmp(instruction, "bgez") == 0 || strcmp(instruction, "bltz") == 0
               || strcmp(instruction, "bgtz") == 0 || strcmp(instruction, "j") == 0
               || strcmp(instruction, "jr") == 0 || strcmp(instruction, "ret") == 0


            ) {
        return 'P';
    } else {
        return 'O';
    }
}

_Bool valid_R_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 4) 
        return 0;

    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_REGISTER);
    return is;
}

_Bool valid_I_bracket_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 6) 
        return 0;

    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_L_PAREN
                && instruction_tokens->list[instruction_tokens->index + 4].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 5].token_type == TOKEN_R_PAREN);
    return is;
}

_Bool valid_I_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 4) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_IMMEDIATE);
    return is;
}

_Bool valid_S_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 6) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_L_PAREN
                && instruction_tokens->list[instruction_tokens->index + 4].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 5].token_type == TOKEN_R_PAREN);
    return is;
}

_Bool valid_B_imm_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 4) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_IMMEDIATE);
    return is;
}

_Bool valid_B_label_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 4) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 3].token_type == TOKEN_LABEL_REF);
    return is;
}

_Bool valid_U_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 3) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE);
    return is;
}

_Bool valid_J_imm_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 3) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_IMMEDIATE);
    return is;
}

_Bool valid_J_label_format(token_list_s *instruction_tokens) {
    if (instruction_tokens->size != instruction_tokens->index + 3) 
        return 0;
    
    _Bool is = (instruction_tokens->list[instruction_tokens->index + 1].token_type == TOKEN_REGISTER
                && instruction_tokens->list[instruction_tokens->index + 2].token_type == TOKEN_LABEL_REF);
    return is;
}


uint32_t R_format_binary_code(R_Type instruction, uint rd, uint rs1, uint rs2) {
    uint opcode = instruction.opcode;
    uint funct3 = instruction.funct3;
    uint funct7 = instruction.funct7;

    uint32_t machine_code = 0;
    machine_code |= (funct7 << 25); // funct7 occupies bits 31-25
    machine_code |= (rs2 << 20);    // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);    // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12); // funct3 occupies bits 14-12
    machine_code |= (rd << 7);      // rd occupies bits 11-7
    machine_code |= opcode;         // opcode occupies bits 6-0

    return machine_code;
}

uint32_t I_format_binary_code(I_Type instruction, uint rd, uint rs1, int imm) {
    uint opcode = instruction.opcode;
    uint funct3 = instruction.funct3;

    uint32_t machine_code = 0;
    machine_code |= (imm << 20);    // imm occupies bits 31-20
    machine_code |= (rs1 << 15);    // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12); // funct3 occupies bits 14-12
    machine_code |= (rd << 7);      // rd occupies bits 11-7
    machine_code |= opcode;         // opcode occupies bits 6-0

    return machine_code;
}

uint32_t S_format_binary_code(S_Type instruction, uint rs1, uint rs2, uint imm) {
    uint opcode = instruction.opcode;
    uint funct3 = instruction.funct3;

    uint imm_11_5 = (imm >> 5) & 0b1111111;
    uint imm_4_0 = imm & 0b11111;

    uint32_t machine_code = 0;
    machine_code |= (imm_11_5 << 25); // imm[11:5] occupies bits 31-25
    machine_code |= (rs2 << 20);      // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);      // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12);   // funct3 occupies bits 14-12
    machine_code |= (imm_4_0 << 7);   // imm[4:0] occupies bits 11-7
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

uint32_t B_format_binary_code(B_Type instruction, uint rs1, uint rs2, int imm) {
    uint opcode = instruction.opcode;
    uint funct3 = instruction.funct3;

    uint imm_12 = (imm >> 12) & 0b1;
    uint imm_10_5 = (imm >> 5) & 0b111111;
    uint imm_4_1 = (imm >> 1) & 0b1111;
    uint imm_11 = (imm >> 11) & 0b1;

    uint32_t machine_code = 0;
    machine_code |= (imm_12 << 31); // imm[12] occupies bit 31
    machine_code |= (imm_10_5 << 25); // imm[10:5] occupies bits 30-25
    machine_code |= (rs2 << 20);      // rs2 occupies bits 24-20
    machine_code |= (rs1 << 15);      // rs1 occupies bits 19-15
    machine_code |= (funct3 << 12);   // funct3 occupies bits 14-12
    machine_code |= (imm_4_1 << 8);   // imm[4:1] occupies bits 11-8
    machine_code |= (imm_11 << 7);    // imm[11] occupies bit 7
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

uint32_t U_format_binary_code(U_Type instruction, uint rd, int imm) {
    uint opcode = instruction.opcode;

    uint32_t machine_code = 0;
    machine_code |= (imm << 12); // imm occupies bits 31-12
    machine_code |= (rd << 7);   // rd occupies bits 11-7
    machine_code |= opcode;      // opcode occupies bits 6-0

    return machine_code;
}

uint32_t J_format_binary_code(J_Type instruction, uint rd, int imm) {
    uint opcode = instruction.opcode;

    uint imm_20 = (imm >> 20) & 0b1;
    uint imm_10_1 = (imm >> 1) & 0b1111111111;
    uint imm_11 = (imm >> 11) & 0b1;
    uint imm_19_12 = (imm >> 12) & 0b11111111;

    uint32_t machine_code = 0;
    machine_code |= (imm_20 << 31); // imm[20] occupies bit 31
    machine_code |= (imm_10_1 << 21); // imm[10:1] occupies bits 30-21
    machine_code |= (rd << 7);        // rd occupies bits 11-7
    machine_code |= (imm_11 << 20);   // imm[11] occupies bit 20
    machine_code |= (imm_19_12 << 12); // imm[19:12] occupies bits 19-12
    machine_code |= opcode;           // opcode occupies bits 6-0

    return machine_code;
}

void generate_bit_string(char *code, uint rd, uint rs1, uint rs2, int imm, input_s *input) {
    uint32_t machine_code;
    if (strcmp(code, "add") == 0) {
        machine_code = R_format_binary_code(add_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "sub") == 0) {
        machine_code = R_format_binary_code(sub_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "xor") == 0) {
        machine_code = R_format_binary_code(xor_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "or") == 0) {
        machine_code = R_format_binary_code(or_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "and") == 0) {
        machine_code = R_format_binary_code(and_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "sll") == 0) {
        machine_code = R_format_binary_code(sll_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "srl") == 0) {
        machine_code = R_format_binary_code(srl_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "sra") == 0) {
        machine_code = R_format_binary_code(sra_instruction, rd, rs1, rs2);
    } else if (strcmp(code, "addi") == 0) {
        machine_code = I_format_binary_code(addi_instruction, rd, rs1, imm);
    } else if (strcmp(code, "xori") == 0) {
        machine_code = I_format_binary_code(xori_instruction, rd, rs1, imm);
    } else if (strcmp(code, "ori") == 0) {
        machine_code = I_format_binary_code(ori_instruction, rd, rs1, imm);
    } else if (strcmp(code, "andi") == 0) {
        machine_code = I_format_binary_code(andi_instruction, rd, rs1, imm);
    } else if (strcmp(code, "slli") == 0) {
        machine_code = I_format_binary_code(slli_instruction, rd, rs1, imm);
    } else if (strcmp(code, "srli") == 0) {
        machine_code = I_format_binary_code(srli_instruction, rd, rs1, imm);
    } else if (strcmp(code, "srai") == 0) {
        machine_code = I_format_binary_code(srai_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lb") == 0) {
        machine_code = I_format_binary_code(lb_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lh") == 0) {
        machine_code = I_format_binary_code(lh_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lw") == 0) {
        machine_code = I_format_binary_code(lw_instruction, rd, rs1, imm);
    } else if (strcmp(code, "ld") == 0) {
        machine_code = I_format_binary_code(ld_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lbu") == 0) {
        machine_code = I_format_binary_code(lbu_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lhu") == 0) {
        machine_code = I_format_binary_code(lhu_instruction, rd, rs1, imm);
    } else if (strcmp(code, "lwu") == 0) {
        machine_code = I_format_binary_code(lwu_instruction, rd, rs1, imm);
    } else if (strcmp(code, "jalr") == 0) {
        machine_code = I_format_binary_code(jalr_instruction, rd, rs1, imm);
    } else if (strcmp(code, "ecall") == 0) {
        machine_code = I_format_binary_code(ecall_instruction, 0, 0, 0);
    } else if (strcmp(code, "sb") == 0) {
        machine_code = S_format_binary_code(sb_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "sh") == 0) {
        machine_code = S_format_binary_code(sh_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "sw") == 0) {
        machine_code = S_format_binary_code(sw_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "sd") == 0) {
        machine_code = S_format_binary_code(sd_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "beq") == 0) {
        machine_code = B_format_binary_code(beq_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "bne") == 0) {
        machine_code = B_format_binary_code(bne_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "blt") == 0) {
        machine_code = B_format_binary_code(blt_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "bge") == 0) {
        machine_code = B_format_binary_code(bge_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "bltu") == 0) {
        machine_code = B_format_binary_code(bltu_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "bgeu") == 0) {
        machine_code = B_format_binary_code(bgeu_instruction, rs1, rs2, imm);
    } else if (strcmp(code, "jal") == 0) {
        machine_code = J_format_binary_code(jal_instruction, rd, imm);
    } else if (strcmp(code, "lui") == 0) {
        machine_code = U_format_binary_code(lui_instruction, rd, imm);
    } else if (strcmp(code, "auipc") == 0) {
        machine_code = U_format_binary_code(auipc_instruction, rd, imm);
    } else {
        printf("Invalid instruction\n");
    }
    write_newline_to_file(output_file_, machine_code);
    input->binary[input->binary_index++] = machine_code;
}
