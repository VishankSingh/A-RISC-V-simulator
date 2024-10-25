/**
 * File Name: stack.c
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/stack.h"
#include "../include/globals.h"

stack_s *init_stack_s(int size) {
    stack_s *stack = (stack_s *)malloc(sizeof(stack_s));
    MEM_INIT_ERROR(stack, init_stack_s);
    stack->data = (lil_node_s *)malloc(size * sizeof(lil_node_s));
    MEM_INIT_ERROR(stack->data, init_stack_s);
    stack->top = -1;
    stack->size = size;
    return stack;
}

void push(stack_s *stack, lil_node_s *node) {
    if (is_full(stack)) {
        printf("Stack is full\n");
        return;
    }
    stack->data[++stack->top] = *node;
}

lil_node_s *pop(stack_s *stack) {
    lil_node_s *node = (lil_node_s *)malloc(sizeof(lil_node_s));
    if (is_empty(stack)) {
        return NULL;
    }
    *node = stack->data[stack->top--];
    return node;
}

lil_node_s *peek(stack_s *stack) {
    lil_node_s *node = (lil_node_s *)malloc(sizeof(lil_node_s));
    if (is_empty(stack)) {
        printf("Stack is empty\n");
        return NULL;
    }
    *node = stack->data[stack->top];
    return node;
}

int is_empty(stack_s *stack) {
    return stack->top == -1;
}

int is_full(stack_s *stack) {
    return stack->top == stack->size - 1;
}

void print_stack(stack_s *stack) {
    if (is_empty(stack)) {
        printf("Empty Call Stack: Execution complete\n");
        return;
    }
    for (int i = 0; i <= stack->top; i++) {
        printf("%s:%d \n", stack->data[i].label, stack->data[i].line_num);
    }
}

void free_stack(stack_s *stack) {
    free(stack->data);
    free(stack);
}