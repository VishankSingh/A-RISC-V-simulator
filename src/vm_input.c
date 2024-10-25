/**
 * File Name: vm_input.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */

#include "../include/vm_input.h"
#include "../include/globals.h"
#include "../include/io.h"


lil_map_s *init_lilmap(uint64_t size) {
    lil_node_s *nodes = (lil_node_s *)malloc(size * sizeof(lil_node_s));
    MEM_INIT_ERROR(nodes, init_lilmap);
    lil_map_s *lilmap = (lil_map_s *)malloc(sizeof(lil_map_s));
    MEM_INIT_ERROR(lilmap, init_lilmap);
    lilmap->lil_nodes = nodes;
    lilmap->index = 0;
    lilmap->size = 0;
    return lilmap;
}

void write_lilmap(lil_map_s *lilmap, uint inst_num, uint line_num, char *label) {
    lilmap->lil_nodes[lilmap->index].inst_num = inst_num;
    lilmap->lil_nodes[lilmap->index].line_num = line_num;
    memcpy(lilmap->lil_nodes[lilmap->index].label, label, strlen(label));
    lilmap->index++;
}

void update_lilmap_line(lil_map_s *lilmap, uint line, char *label) {
    int low = 0;
    int high = lilmap->index - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (lilmap->lil_nodes[mid].line_num == line) {
            memcpy(lilmap->lil_nodes[mid].label, label, strlen(label) + 1);
            return;
        } else if (lilmap->lil_nodes[mid].line_num < line) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
}


lil_node_s *get_lilnode_label(lil_map_s *lilmap, char *label) {
    for (uint i = 0; i < lilmap->index; i++) {
        if (strcmp(lilmap->lil_nodes[i].label, label) == 0) {
            return &lilmap->lil_nodes[i];
        }
    }
    return NULL;
}

lil_node_s *get_lilnode_instnum(lil_map_s *lilmap, uint instnum) {
    int low = 0;
    int high = lilmap->index - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (lilmap->lil_nodes[mid].inst_num == instnum)
            return &lilmap->lil_nodes[mid];
        else if (lilmap->lil_nodes[mid].inst_num < instnum)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return NULL;
}

int get_linenum_instnum(lil_map_s *lilmap, uint instnum) {
    int low = 0;
    int high = lilmap->index - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (lilmap->lil_nodes[mid].inst_num == instnum)
            return lilmap->lil_nodes[mid].line_num;
        else if (lilmap->lil_nodes[mid].inst_num < instnum)
            low = mid + 1;
        else
            high = mid - 1;
    }
    
    return -1;
}

int get_linenum_label(lil_map_s *lilmap, char *label) {
    for (uint i = 0; i < lilmap->index; i++) {
        if (strcmp(lilmap->lil_nodes[i].label, label) == 0) {
            return lilmap->lil_nodes[i].line_num;
        }
    }
    return -1;
}

int get_instnum_linenum(lil_map_s *lilmap, uint linenum) {
    int low = 0;
    int high = lilmap->index - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (lilmap->lil_nodes[mid].line_num == linenum)
            return lilmap->lil_nodes[mid].inst_num;
        else if (lilmap->lil_nodes[mid].line_num < linenum)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}


int get_instnum_label(lil_map_s *lilmap, char *label) {
    for (uint i = 0; i < lilmap->index; i++) {
        if (strcmp(lilmap->lil_nodes[i].label, label) == 0) {
            return lilmap->lil_nodes[i].inst_num;
        }
    }
    return -1;
}

void print_lilmap(lil_map_s *lilmap) {
    for (uint i = 0; i < lilmap->index; i++) {
        printf("%d: %d: %s\n", lilmap->lil_nodes[i].line_num, lilmap->lil_nodes[i].inst_num,
               lilmap->lil_nodes[i].label);
    }
}

void free_lilmap(lil_map_s *lilmap) {
    free(lilmap->lil_nodes);
    free(lilmap);
}

input_s *init_input_s(uint64_t size, char *filename) {
    input_s *input = (input_s *)malloc(sizeof(input_s));
    MEM_INIT_ERROR(input, init_input_s);
    int line_count;
    input->binary = (uint32_t *)malloc(size * sizeof(uint32_t));
    MEM_INIT_ERROR(input->binary, init_input_s);
    input->binary_index = 0;
    input->size = 0;
    input->lil_map = init_lilmap(size);
    MEM_INIT_ERROR(input->lil_map, init_input_s);
    input->file = NULL;

    char* buffer = (char *)malloc(1024 * sizeof(char));
    if (*filename != '\0') {
        line_count = count_lines_in_file(filename);
        input->file = (char *)malloc(1024 * line_count * sizeof(char));
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("Error opening file");
            return NULL;
        }

        int i = 0;
        while (fgets(buffer, sizeof(buffer), fp) != NULL && i < line_count) {
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(&input->file[i * 1024], buffer); // Copy buffer to the right position
            input->size++;
            i++;
        }
        fclose(fp);

        //for (int i = 0; i < line_count; i++) {
        //    printf("%2d: %s\n", i + 1, &input->file[i * 1024]);
        //}
    }
    free(buffer);

    return input;
}

void write_input(input_s *input, uint32_t instruction) {
    input->binary[input->binary_index++] = instruction;
}

void print_input(input_s *input) {
    for (uint i = 0; i < input->binary_index; i++) {
        printf("%d: ", i);
        print_binary(input->binary[i]);
        printf("\n");
        // printf("%d: %d\n", i, input->binary[i]);
    }
}

void *reset_input(input_s *input) {
    free(input->binary);
    input->binary = (uint32_t *)malloc(input->size * sizeof(uint32_t));
    MEM_INIT_ERROR(input->binary, reset_input);
    input->binary_index = 0;
    return NULL;
}

void free_input(input_s *input) {
    if (input) {
        if (input->binary) free(input->binary);
        if (input->lil_map) free_lilmap(input->lil_map);
        free(input);
    }
}