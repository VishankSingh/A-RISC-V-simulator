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
    const size_t BUFFER_SIZE = 8194;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    int lines = 0;
    size_t bytes_read;

    // Read the file in chunks
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        // Count the number of newlines in the buffer
        for (size_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                lines++;
            }
        }
    }
    lines++;  // Add one more line for the last line

    fclose(file);
    return lines;
}
