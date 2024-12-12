/**
 * File Name: assembler.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./vm.h"

void assemble(char *output_file, vm_s *vm);

#endif // ASSEMBLE_H
