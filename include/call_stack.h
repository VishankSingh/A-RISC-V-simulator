/**
 * File Name: stack.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef CALL_STACK_H
#define CALL_STACK_H

#include "vm_input.h"

typedef struct Stack {
    lil_node_s *data;
    int top;
    int size;
} stack_s;

stack_s *init_stack_s(int size);

void push(stack_s *stack, lil_node_s *node);

lil_node_s *pop(stack_s *stack);

lil_node_s *peek(stack_s *stack);

int is_empty(stack_s *stack);

int is_full(stack_s *stack);

void print_stack(stack_s *stack);

void free_stack(stack_s *stack);

#endif // CALL_STACK_H