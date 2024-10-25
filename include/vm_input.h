/**
 * File Name: vm_input.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef VM_INPUT_H
#define VM_INPUT_H

#include "utils.h"
#include "globals.h"

#define uint unsigned int

typedef struct lil_node {
    uint inst_num;
    uint line_num;
    char label[MAX_LABEL_SIZE];
} lil_node_s;

typedef struct lil_map {
    lil_node_s *lil_nodes;
    uint index;
    uint size;
} lil_map_s;

lil_map_s *init_lilmap(uint64_t size);

void write_lilmap(lil_map_s *lilmap, uint inst_num, uint line_no, char *label);

void update_lilmap_line(lil_map_s *lilmap, uint line, char *label);

lil_node_s *get_lilnode_label(lil_map_s *lilmap, char *label);

lil_node_s *get_lilnode_instnum(lil_map_s *lilmap, uint instnum);

int get_linenum_instnum(lil_map_s *lilmap, uint instnum);

int get_linenum_label(lil_map_s *lilmap, char *label);

int get_instnum_linenum(lil_map_s *lilmap, uint linenum);

int get_instnum_label(lil_map_s *lilmap, char *label);

void print_lilmap(lil_map_s *lilmap);

void free_lilmap(lil_map_s *lilmap);

typedef struct Input {
    uint32_t *binary;
    uint binary_index;
    char *file;
    lil_map_s *lil_map;
    uint size;
} input_s;

input_s *init_input_s(uint64_t size, char *filename);

void write_input(input_s *input, uint32_t instruction);

void print_input(input_s *input);

void * reset_input(input_s *input);

void free_input(input_s *input);


#endif // VM_INPUT_H