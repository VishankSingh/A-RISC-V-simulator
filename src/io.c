/**
 * File Name: io.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */


#include "../include/io.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_file(char *filename) {
    FILE *file = fopen(filename, "w");
    fclose(file);
}

void write_newline_to_file(char *filename, uint32_t machine_code) {
    FILE *file = fopen(filename, "a");
    fprintf(file, "%08x\n", machine_code);
    fclose(file);
}

char *read_line(char *filename, uint line_number) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("[-] Error: Unable to opedn file %s\n", filename);
        return NULL;
    }
    char *line = (char *)malloc(1024 * sizeof(char));
    uint count = 0;
    while (fgets(line, 1024, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        line[strcspn(line, "#")] = 0;
        line[strcspn(line, ";")] = 0;
        ++count;
        if (count == line_number) break;
    }
    while (line[0] == ' ' || line[0] == '\t') {
        line++;
    }

    return line;
}

int count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return -1;
    }

    int count = 0;
    char ch;
    int prev_char_was_newline = 1;  // Handles case where the file is empty or doesn't start with '\n'

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++;
            prev_char_was_newline = 1;
        } else {
            prev_char_was_newline = 0;
        }
    }

    // If the last character is not '\n' but file has content, count the last line
    if (!prev_char_was_newline) {
        count++;
    }

    fclose(file);
    return count;
}
