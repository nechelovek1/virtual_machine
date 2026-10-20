#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif

#ifdef CANARY
#define ON_CANARY(...) __VA_ARGS__
#else
#define ON_CANARY(...)
#endif

typedef int stack_elem_t;

typedef enum {
    STACK_OK,
    STACK_EMPTY,
    STACK_BAD_ALLOC,
    STACK_NULL_POINTER,
    STACK_ALREADY_EXIST,
    STACK_BAD_SIZE,
    STACK_BAD_DATA,
    STACK_POISON_IN_DATA,
    STACK_BAD_CAPACITY,
    STACK_NEGATIVE_SIZE,
    STACK_NEGATIVE_CAPACITY,
    STACK_CANARY_LEFT_ERR,
    STACK_CANARY_RIGHT_ERR,
    BUFFER_CANARY_LEFT_ERR,
    BUFFER_CANARY_RIGHT_ERR   
} stack_error_t;

typedef struct {
    ON_CANARY(long canary_left;)
    ON_CANARY(stack_elem_t* buffer_canary_left;)
    stack_elem_t* buffer;
    stack_elem_t* data;
    ON_CANARY(stack_elem_t* buffer_canary_right;)
    ssize_t size;
    ssize_t capacity;
#ifdef DEBUG
    stack_error_t error;
#elifdef CANARY
    stack_error_t error;
#endif
    ON_DBG(
        const char* file_name;
        const char* func_name;
        size_t line;
        const char* name;
    )
    ON_CANARY(long canary_right;)
} stack_t;

void stack_dump(stack_t* stk);
void stack_print_error(stack_error_t err);

stack_error_t stack_init(stack_t* stk, size_t initial_capacity 
    ON_DBG(, const char* file_name, const char* func_name, size_t line, const char* name));
stack_error_t stack_push(stack_t* stk, stack_elem_t elem);
stack_error_t stack_pop(stack_t* stk, stack_elem_t* elem);
stack_error_t stack_destroy(stack_t* stk);
stack_error_t stack_verifyer(stack_t* stk);
stack_error_t stack_isclear(stack_t* stk);
stack_error_t stack_canary(stack_t* stk);

#endif