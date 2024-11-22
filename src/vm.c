/**
 * File Name: vm.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include "../include/vm.h"
#include "../include/globals.h"

vm_s *init_vm_s(char *filename) {
    vm_s *vm = (vm_s *)malloc(sizeof(vm_s));
    MEM_INIT_ERROR(vm, init_vm_s);
    if (filename != NULL) {
        strncpy(vm->filename, filename, 100 - 1);
        vm->filename[100 - 1] = '\0';
    } else {
        vm->filename[0] = '\0';
    }

    vm->input = init_input_s(MAX_INPUT_FILE_SIZE, vm->filename);
    if (vm->input == NULL) {
        return NULL;
    }
    vm->register_file = init_registers_s();
    if (vm->register_file == NULL) {
        return NULL;
    }
    vm->memory = init_memory_s(MAX_MEMORY_SIZE);
    if (vm->memory == NULL) {
        return NULL;
    }
    vm->lookup_table = init_lookup_table();
    vm->breakpoints = (uint64_t *)malloc(MAX_BREAKPOINTS * sizeof(uint64_t));
    MEM_INIT_ERROR(vm->breakpoints, init_vm_s:
        breakpoints);
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        vm->breakpoints[i] = INT64_MAX;
    }
    vm->pc = 0x00000;
    vm->last_pc = vm->pc - 4;
    vm->call_stack = init_stack_s(CALL_STACK_SIZE);
    vm->cache = init_cache();
    disable_cache(vm->cache);
    return vm;
}


void vm_reset(vm_s *vm, char *filename) {
    strncpy(vm->filename, filename, 100 - 1);
    vm->filename[100 - 1] = '\0';
    vm->input = init_input_s(MAX_INPUT_FILE_SIZE, vm->filename);
    reset_registers(vm->register_file);
    reset_memory(vm->memory);
    vm->pc = 0x00000;
    vm->last_pc = vm->pc - 4;
    vm->call_stack = init_stack_s(CALL_STACK_SIZE);
    _Bool cache_status = vm->cache->enabled;
    vm->cache = init_cache();
    //if (cache_status) enable_cache(vm->cache, "cache.config", vm->cache->cache_output_file);
    error_code_ = 0;
    
}

void add_breakpoint(vm_s *vm, uint64_t address) {
    _Bool added = 0;
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        if (vm->breakpoints[i] == INT64_MAX) {
            vm->breakpoints[i] = address;
            added = 1;
            break;
        }
    }
    if (!added) {
        return;
    }
}

void remove_breakpoint(vm_s *vm, uint64_t address) {
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        if (vm->breakpoints[i] == address) {
            vm->breakpoints[i] = INT64_MAX;
            break;
        }
    }
}

_Bool in_breakpoint(vm_s *vm, uint64_t address) {
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
        if (vm->breakpoints[i] == address) {
            return 1;
        }
    }
    return 0;
}

void toggle_breakpoint(vm_s *vm, uint64_t address) {
    if (in_breakpoint(vm, address)) {
        remove_breakpoint(vm, address);
    } else {
        add_breakpoint(vm, address);
    }
}

void vm_run(vm_s *vm, _Bool run) {
    if (run) {
        int i = 0;
        while (!in_breakpoint(vm, vm->pc) && i < MAX_LOOP_ITERATIONS) {
            vm_execute_instruction(vm, read_instruction(vm->memory, vm->pc));
            i++;
        }
        if (vm->pc == vm->breakpoints[0]) {
            pop(vm->call_stack);
            return;
        }
        if (in_breakpoint(vm, vm->pc)) {
        }
    } else {
        if (vm->pc != vm->breakpoints[0])
            vm_execute_instruction(vm, read_instruction(vm->memory, vm->pc));
        else {
            pop(vm->call_stack);
        }
    }
}

int opcode_hash(int opcode) {
    switch (opcode) {
        case 0x03:
            return 0;
        case 0x13:
            return 1;
        case 0x17:
            return 2;
        case 0x23:
            return 3;
        case 0x33:
            return 4;
        case 0x37:
            return 5;
        case 0x63:
            return 6;
        case 0x67:
            return 7;
        case 0x6f:
            return 8;
        default:
            return 9;
    }
}

int funct7_hash(int funct7) {
    switch (funct7) {
        case 0x00:
            return 0;
        case 0x20:
            return 1;
        default:
            return 2;
    }
}

void vm_execute_instruction(vm_s *vm, uint32_t instruction) {
    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = (instruction >> 12) & 0x7;
    uint8_t funct6 = (instruction >> 30) & 0x3;
    uint8_t funct7 = (instruction >> 25) & 0x7F;
    if (vm->lookup_table->opcode_table[opcode_hash(opcode)] != NULL) {
        vm->call_stack->data[vm->call_stack->top].inst_num = vm->pc / 4 + 1;
        vm->call_stack->data[vm->call_stack->top].line_num = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
        vm->lookup_table->opcode_table[opcode_hash(opcode)](instruction, vm);
    } 
    else if (vm->lookup_table->funct3_table[opcode_hash(opcode)][funct3] != NULL) {
        vm->call_stack->data[vm->call_stack->top].inst_num = vm->pc / 4 + 1;
        vm->call_stack->data[vm->call_stack->top].line_num = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
        vm->lookup_table->funct3_table[opcode_hash(opcode)][funct3](instruction, vm);
    } 
    else if (vm->lookup_table->funct6_table[opcode_hash(opcode)][funct3][funct6] != NULL) {
        vm->call_stack->data[vm->call_stack->top].inst_num = vm->pc / 4 + 1;
        vm->call_stack->data[vm->call_stack->top].line_num = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
        vm->lookup_table->funct6_table[opcode_hash(opcode)][funct3][funct6](instruction, vm);
    } 
    else if (vm->lookup_table->funct7_table[opcode_hash(opcode)][funct3][funct7_hash(funct7)] != NULL) {
        vm->call_stack->data[vm->call_stack->top].inst_num = vm->pc / 4 + 1;
        vm->call_stack->data[vm->call_stack->top].line_num = get_linenum_instnum(vm->input->lil_map, vm->pc / 4 + 1);
        vm->lookup_table->funct7_table[opcode_hash(opcode)][funct3][funct7_hash(funct7)](instruction, vm);
    } 
    else {
        return;
    }
}

void free_vm_s(vm_s *vm) {
    free_input(vm->input);
    free_registers(vm->register_file);
    free_memory(vm->memory);
    free_lookup_table(vm->lookup_table);
    free(vm->breakpoints);
    free_stack(vm->call_stack);
    free(vm);
}


lookup_table_s *init_lookup_table() {
    lookup_table_s *lookup_table = (lookup_table_s *)malloc(sizeof(lookup_table_s));
    MEM_INIT_ERROR(lookup_table, init_lookup_table);
    lookup_table->funct7_table[4][0x0][0] = &execute_add;
    lookup_table->funct7_table[4][0x0][1] = &execute_sub;
    lookup_table->funct7_table[4][0x4][0] = &execute_xor;
    lookup_table->funct7_table[4][0x6][0] = &execute_or;
    lookup_table->funct7_table[4][0x7][0] = &execute_and;
    lookup_table->funct7_table[4][0x1][0] = &execute_sll;
    lookup_table->funct7_table[4][0x5][0] = &execute_srl;
    lookup_table->funct7_table[4][0x5][1] = &execute_sra;

    lookup_table->funct3_table[1][0x0] = &execute_addi;
    lookup_table->funct3_table[1][0x4] = &execute_xori;
    lookup_table->funct3_table[1][0x6] = &execute_ori;
    lookup_table->funct3_table[1][0x7] = &execute_andi;
    lookup_table->funct6_table[1][0x1][0x00] = &execute_slli;
    lookup_table->funct6_table[1][0x5][0x00] = &execute_srli;
    lookup_table->funct6_table[1][0x5][0x01] = &execute_srai;
    // lookup_table->funct3_table[1][0x2]       = &execute_slti;
    // lookup_table->funct3_table[1][0x3]       = &execute_sltiu;

    lookup_table->funct3_table[0][0x0] = &execute_lb;
    lookup_table->funct3_table[0][0x1] = &execute_lh;
    lookup_table->funct3_table[0][0x2] = &execute_lw;
    lookup_table->funct3_table[0][0x3] = &execute_ld;
    lookup_table->funct3_table[0][0x4] = &execute_lbu;
    lookup_table->funct3_table[0][0x5] = &execute_lhu;
    lookup_table->funct3_table[0][0x6] = &execute_lwu;

    lookup_table->funct3_table[3][0x0] = &execute_sb;
    lookup_table->funct3_table[3][0x1] = &execute_sh;
    lookup_table->funct3_table[3][0x2] = &execute_sw;
    lookup_table->funct3_table[3][0x3] = &execute_sd;

    lookup_table->funct3_table[6][0x0] = &execute_beq;
    lookup_table->funct3_table[6][0x1] = &execute_bne;
    lookup_table->funct3_table[6][0x4] = &execute_blt;
    lookup_table->funct3_table[6][0x5] = &execute_bge;
    lookup_table->funct3_table[6][0x6] = &execute_bltu;
    lookup_table->funct3_table[6][0x7] = &execute_bgeu;

    lookup_table->opcode_table[8] = &execute_jal;
    lookup_table->funct3_table[7][0x0] = &execute_jalr;

    lookup_table->opcode_table[5] = &execute_lui;
    lookup_table->opcode_table[2] = &execute_auipc;
    return lookup_table;
}

void free_lookup_table(lookup_table_s *lookup_table) {
    if (lookup_table)
        free(lookup_table);
}

void execute_add(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] + vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sub(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] - vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_xor(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] ^ vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_or(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] | vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_and(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] & vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sll(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = vm->register_file->reg_arr[rs1] << vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_srl(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = (uint64_t)vm->register_file->reg_arr[rs1] >> vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sra(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    uint64_t result = (int64_t) vm->register_file->reg_arr[rs1] >> vm->register_file->reg_arr[rs2];
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_addi(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->register_file->reg_arr[rs1] + imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_slti(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->register_file->reg_arr[rs1] < imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sltiu(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = (uint64_t) vm->register_file->reg_arr[rs1] < (uint64_t) imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_xori(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->register_file->reg_arr[rs1] ^ imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_ori(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->register_file->reg_arr[rs1] | imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_andi(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->register_file->reg_arr[rs1] & imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lb(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    int8_t value = 0;


    if (vm->cache->enabled) {

        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);

        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset];
    }
    else {
        value = read_memory(vm->memory, address);
    }

    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lh(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    int16_t value = 0;

    if (vm->cache->enabled) {

        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);

        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset] | data[byte_offset + 1] << 8;
    }
    else {
        value = read_memory(vm->memory, address)
                 | read_memory(vm->memory, address + 1) << 8;
    }

    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lw(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    int32_t value = 0;

    if (vm->cache->enabled) {
        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);

        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset] | data[byte_offset + 1] << 8
                 | data[byte_offset + 2] << 16 | data[byte_offset + 3] << 24;
    }
    else {
        value = read_memory(vm->memory, address)
                 | read_memory(vm->memory, address + 1) << 8
                 | read_memory(vm->memory, address + 2) << 16
                 | read_memory(vm->memory, address + 3) << 24;
    }

    

    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_ld(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    int64_t value = 0;

    if (vm->cache->enabled) {
        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);


        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        for (int i = 0; i < 8; i++) {
            value |= data[byte_offset + i] << (i * 8);
        }
    }
    else {
        value = read_memory(vm->memory, address)
                 | read_memory(vm->memory, address + 1) << 8
                 | read_memory(vm->memory, address + 2) << 16
                 | read_memory(vm->memory, address + 3) << 24
                 | read_memory(vm->memory, address + 4) << 32
                 | read_memory(vm->memory, address + 5) << 40
                 | read_memory(vm->memory, address + 6) << 48
                 | read_memory(vm->memory, address + 7) << 56;
    }


    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lbu(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;

    uint8_t value = 0;
    if (vm->cache->enabled) {

        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);
        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset];
    }
    else {
        value = read_memory(vm->memory, address);
    }

    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lhu(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    
    uint16_t value = 0;

    if (vm->cache->enabled) {

        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);
        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset] | data[byte_offset + 1] << 8;
    }
    else {
        value = read_memory(vm->memory, address)
                 | read_memory(vm->memory, address + 1) << 8;
    }

    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_lwu(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    
    uint32_t value = 0;

    if (vm->cache->enabled) {

        uint8_t *data = access_cache(vm->cache, address, vm->memory, vm->cache->cache_output_file);
        uint64_t byte_offset = address & ((1 << vm->cache->byte_offset_bits) - 1);
        value = data[byte_offset] | data[byte_offset + 1] << 8
                 | data[byte_offset + 2] << 16 | data[byte_offset + 3] << 24;
    }
    else {
        value = read_memory(vm->memory, address)
                 | read_memory(vm->memory, address + 1) << 8
                 | read_memory(vm->memory, address + 2) << 16
                 | read_memory(vm->memory, address + 3) << 24;
    }


    write_registers(vm->register_file, rd, value);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sb(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 7) & 0x1F)
                  | ((instruction >> 25) & 0x7F) << 5;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    uint8_t value = vm->register_file->reg_arr[rs2] & 0xFF;

    if (vm->cache->enabled) {
        uint8_t *value_ptr = malloc(1 * sizeof(uint8_t));
        value_ptr[0] = value;

        write_cache(vm->cache, address, value_ptr, vm->memory, 1, vm->cache->cache_output_file);
    }
    else {
        write_memory(vm->memory, address, value, 1);
    }

    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sh(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 7) & 0x1F)
                  | ((instruction >> 25) & 0x7F) << 5;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    uint16_t value = vm->register_file->reg_arr[rs2] & 0xFFFF;

    if (vm->cache->enabled) {
        uint8_t *value_ptr = malloc(2 * sizeof(uint8_t));
        for (int i = 0; i < 2; i++) {
            value_ptr[i] = (value >> (i * 8)) & 0xFF;
        }

        write_cache(vm->cache, address, value_ptr, vm->memory, 2, vm->cache->cache_output_file);
    }
    else {
        write_memory(vm->memory, address, value & 0xFF, 1);
        write_memory(vm->memory, address + 1, (value >> 8) & 0xFF, 1);
    }

    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sw(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 7) & 0x1F)
                  | ((instruction >> 25) & 0x7F) << 5;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    uint32_t value = vm->register_file->reg_arr[rs2];

    if (vm->cache->enabled) {
        uint8_t *value_ptr = malloc(4 * sizeof(uint8_t));
        for (int i = 0; i < 4; i++) {
            value_ptr[i] = (value >> (i * 8)) & 0xFF;
        }

        write_cache(vm->cache, address, value_ptr, vm->memory, 4, vm->cache->cache_output_file);
    }
    else {
        write_memory(vm->memory, address, value & 0xFF, 1);
        write_memory(vm->memory, address + 1, (value >> 8) & 0xFF, 1);
        write_memory(vm->memory, address + 2, (value >> 16) & 0xFF, 1);
        write_memory(vm->memory, address + 3, (value >> 24) & 0xFF, 1);
    }

    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_sd(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 7) & 0x1F)
                  | ((instruction >> 25) & 0x7F) << 5;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t address = vm->register_file->reg_arr[rs1] + imm;
    uint64_t value = vm->register_file->reg_arr[rs2];

    if (vm->cache->enabled) {
        uint8_t *value_ptr = malloc(8 * sizeof(uint8_t));
        for (int i = 0; i < 8; i++) {
            value_ptr[i] = (value >> (i * 8)) & 0xFF;
        }
        
        write_cache(vm->cache, address, value_ptr, vm->memory, 8, vm->cache->cache_output_file);
    }
    else {
        write_memory(vm->memory, address, value & 0xFF, 1);
        write_memory(vm->memory, address + 1, (value >> 8) & 0xFF, 1);
        write_memory(vm->memory, address + 2, (value >> 16) & 0xFF, 1);
        write_memory(vm->memory, address + 3, (value >> 24) & 0xFF, 1);
        write_memory(vm->memory, address + 4, (value >> 32) & 0xFF, 1);
        write_memory(vm->memory, address + 5, (value >> 40) & 0xFF, 1);
        write_memory(vm->memory, address + 6, (value >> 48) & 0xFF, 1);
        write_memory(vm->memory, address + 7, (value >> 56) & 0xFF, 1);
    }
    

    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_slli(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t shift = (instruction >> 20) & 0x3F;
    uint64_t result = (uint64_t)vm->register_file->reg_arr[rs1] << shift;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_srli(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t shift = (instruction >> 20) & 0x3F;
    
    uint64_t result = (uint64_t)vm->register_file->reg_arr[rs1] >> shift;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_srai(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t shift = (instruction >> 20) & 0x3F;
    uint64_t result = (int64_t) vm->register_file->reg_arr[rs1] >> shift;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_jal(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    int32_t imm = ((instruction >> 21) & 0x3FF)
                  | ((instruction >> 20) & 0x1) << 10
                  | ((instruction >> 12) & 0xFF) << 11
                  | ((instruction >> 31) & 0x1) << 19;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    uint64_t result = vm->pc + 4;
    write_registers(vm->register_file, rd, result);
    lil_node_s *node = get_lilnode_instnum(vm->input->lil_map, vm->pc / 4 + 1);
    lil_node_s *node_push = (lil_node_s *)malloc(sizeof(lil_node_s));
    memcpy(node_push->label, node->label, 30 * sizeof(char));
    node_push->label[30] = '\0';


    node_push->inst_num = 0;
    node_push->line_num = get_linenum_instnum(vm->input->lil_map, (vm->pc + imm) / 4 + 1) - 1;
    push(vm->call_stack, node_push);

    vm->last_pc = vm->pc;
    vm->pc += imm;
}

void execute_jalr(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (instruction >> 20) & 0xFFF;
    if (imm & 0x800) {
        imm |= 0xFFFFF000;
    }
    uint64_t result = vm->pc + 4;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc = vm->register_file->reg_arr[rs1] + imm;
    pop(vm->call_stack);
}

void execute_beq(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if (vm->register_file->reg_arr[rs1] == vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}

void execute_bne(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if (vm->register_file->reg_arr[rs1] != vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}

void execute_blt(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if ((int64_t) vm->register_file->reg_arr[rs1] < (int64_t) vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}

void execute_bge(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if ((int64_t) vm->register_file->reg_arr[rs1] >= (int64_t) vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}

// TODO: check if this correct
void execute_bltu(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if (vm->register_file->reg_arr[rs1] < vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}


// TODO: check if this is correct
void execute_bgeu(uint32_t instruction, vm_s *vm) {
    uint8_t rs1 = (instruction >> 15) & 0x1F;
    uint8_t rs2 = (instruction >> 20) & 0x1F;
    int32_t imm = ((instruction >> 8) & 0xF)
                  | ((instruction >> 25) & 0x3F) << 4
                  | ((instruction >> 7) & 0x1) << 10
                  | ((instruction >> 31) & 0x1) << 11;
    imm <<= 1;
    if (imm & 0x1000) {
        imm |= 0xFFFFE000;
    }
    vm->last_pc = vm->pc;
    if (vm->register_file->reg_arr[rs1] >= vm->register_file->reg_arr[rs2]) {
        vm->pc += imm;
    } else {
        vm->pc += 4;
    }
}

void execute_lui(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint32_t imm = instruction & 0xFFFFF000;
    uint64_t result = imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_auipc(uint32_t instruction, vm_s *vm) {
    uint8_t rd = (instruction >> 7) & 0x1F;
    uint32_t imm = instruction & 0xFFFFF000;
    uint64_t result = vm->pc + imm;
    write_registers(vm->register_file, rd, result);
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_ecall(uint32_t instruction, vm_s *vm) {
    (void) instruction;
    printf("ECALL\n");
    vm->last_pc = vm->pc;
    vm->pc += 4;
}

void execute_ebreak(uint32_t instruction, vm_s *vm) {
    (void) instruction;
    printf("EBREAK\n");
    vm->last_pc = vm->pc;
    vm->pc += 4;
}




