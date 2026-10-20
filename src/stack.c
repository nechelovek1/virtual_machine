#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "stack.h"

const ssize_t STACK_INCR = 2;
const ssize_t STACK_DECR = STACK_INCR * STACK_INCR;

const stack_elem_t STACK_POISON = 0xFEB01;
const long STACK_CANARY_LEFT = 0xD01B0EB;
const long STACK_CANARY_RIGHT = 0xEBA1A4;
const stack_elem_t BUFFER_CANARY_LEFT = 0xBADEDA;
const stack_elem_t BUFFER_CANARY_RIGHT = 0xBADE1DA;

stack_error_t stack_init(stack_t* stk, size_t initial_capacity 
    ON_DBG(, const char* file_name, const char* func_name, size_t line, const char* name))
{
    ON_DBG(
    if (stk != NULL) {
        stk->file_name = file_name;
        stk->func_name = func_name;
        stk->line = line;
        stk->name = name;
    }
    )

    ON_DBG(assert(stack_isclear(stk) == STACK_OK);)
    
    stk->capacity = initial_capacity;
    stack_elem_t* tmp = (stack_elem_t*) calloc(stk->capacity + 2, sizeof(stack_elem_t));

    if (tmp == NULL) {
        ON_DBG(
            stk->error = STACK_BAD_ALLOC;
            stack_dump(stk);
        )
        return STACK_BAD_ALLOC;
    }
    
    stk->buffer = tmp;
    stk->data = tmp + 1;

    ON_CANARY(
    stk->canary_left = STACK_CANARY_LEFT;
    stk->canary_right = STACK_CANARY_RIGHT;

    stk->buffer_canary_left = stk->buffer;
    stk->buffer_canary_right = stk->data + stk->capacity;

    *(stk->buffer_canary_left) = BUFFER_CANARY_LEFT;
    *(stk->buffer_canary_right) = BUFFER_CANARY_RIGHT;
    )

    ON_DBG(
    for (ssize_t i = 0; i < stk->capacity; i++) {
        stk->data[i] = STACK_POISON;
    }
    )

    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )
    return STACK_OK;
}

stack_error_t stack_push(stack_t* stk, stack_elem_t elem)
{
    stack_error_t err = STACK_OK;

    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )

    if(stk->size == stk->capacity) {
        stk->capacity = stk->capacity * STACK_INCR;
        stack_elem_t* tmp = (stack_elem_t*) realloc(stk->buffer, (stk->capacity + 2) * sizeof(stack_elem_t));

        if (tmp == NULL) {
            ON_DBG(
            stk->error = STACK_BAD_ALLOC;
            stack_dump(stk);
            )

            return STACK_BAD_ALLOC;
        }

        stk->buffer = tmp;
        stk->data = tmp + 1;

        ON_CANARY(
        stk->buffer_canary_left = tmp;
        stk->buffer_canary_right = tmp + stk->capacity + 1;

        *(stk->buffer_canary_left) = BUFFER_CANARY_LEFT;
        *(stk->buffer_canary_right) = BUFFER_CANARY_RIGHT;
        )

    }

    stk->data[stk->size] = elem;
    stk->size++;

    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )

    return STACK_OK;
}

stack_error_t stack_pop(stack_t* stk, stack_elem_t* elem)
{
    stack_error_t err = STACK_OK;

    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )

    if (stk->size == 0) {
        ON_DBG(
        stk->error = STACK_EMPTY;
        stack_dump(stk);
        )

        return STACK_EMPTY;
    }

    stk->size = stk->size - 1;
    *elem = stk->data[stk->size];
    stk->data[stk->size] = STACK_POISON;

    if (stk->capacity / STACK_DECR > stk->size) {
        stk->capacity = stk->capacity / STACK_DECR;
        stack_elem_t* tmp = (stack_elem_t*) realloc(stk->buffer, (stk->capacity + 2) * sizeof(stack_elem_t));

        if (tmp == NULL) {
            ON_DBG(
            stk->error = STACK_BAD_ALLOC;
            stack_dump(stk);
            )

            return STACK_BAD_ALLOC;
        }

        stk->buffer = tmp;
        stk->data = tmp + 1;

        ON_CANARY(
        stk->buffer_canary_left = tmp;
        stk->buffer_canary_right = tmp + stk->capacity + 1;

        *(stk->buffer_canary_left) = BUFFER_CANARY_LEFT;
        *(stk->buffer_canary_right) = BUFFER_CANARY_RIGHT;
        )
    } 

    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )

    return STACK_OK;
}

stack_error_t stack_destroy(stack_t* stk)
{
    ON_DBG(
    if (stack_verifyer(stk) != STACK_OK) {
        return stk->error;
    }
    )

    ON_CANARY(
    if (stack_canary(stk) != STACK_OK) {
        return stk->error;
    }
    )

    free(stk->buffer);
    *stk = (stack_t){};

    return STACK_OK;
}

void stack_dump(stack_t* stk)
{
    if (stk == NULL) {
        printf("Stack pointer is NULL!!\n");
        return;
    }
    
    ON_DBG(
    printf("Stack named <%s> created by <%s> in file <%s> on line %lu\n", 
        stk->name, stk->func_name, stk->file_name, stk->line);
    stack_print_error(stk->error);
    )
    
    printf("[%p]\n", stk);
    ON_CANARY(printf("Stack left canary: %lx\n", stk->canary_left);)
    printf("Stack capacity: %lu\n", stk->capacity);
    printf("Stack size: %lu\n", stk->size);
    ON_CANARY(printf("Stack right canary: %lx\n", stk->canary_right);)

    printf("Stack data addr: %p\n", stk->data);
    if (stk->data == NULL) {
        return;
    }

    for (ssize_t i = 0; i < stk->capacity; i++) {
        printf("[%lu] %d\n", i, stk->data[i]);
    }
}

stack_error_t stack_verifyer(stack_t* stk)
{
    if (stk == NULL) {
        printf("Stack pointer is NULL!!!!!!!\n");
        return STACK_NULL_POINTER;
    }

    ON_DBG(
    if (stk->capacity < stk->size) {
        stk->error = STACK_BAD_CAPACITY;
    } else if (stk->capacity <= 0) {
        stk->error = STACK_NEGATIVE_CAPACITY;
    } else if (stk->size < 0) {
        stk->error = STACK_NEGATIVE_SIZE;
    } else if (stk->buffer == NULL) {
        stk->error = STACK_BAD_DATA;
    } /*else for (ssize_t i = 0; i < stk->size; i++) {
        if (stk->data[i] == STACK_POISON) {
            stk->error = STACK_POISON_IN_DATA;
            break;
        }
    }*/

    if (stk->error != STACK_OK) {
        stack_dump(stk);
    }

    return stk->error;
    )

    return STACK_OK;
}

stack_error_t stack_isclear(stack_t* stk)
{
    stack_error_t err = STACK_OK;

    if (stk == NULL) {
        err = STACK_NULL_POINTER;
    } else if (stk->data == NULL && stk->capacity == 0 && stk->size == 0) {
        err = STACK_OK;
    } else {
        err = STACK_ALREADY_EXIST;
    }

    if (err != STACK_OK) {
        stack_dump(stk);
    }

    return err;
}

void stack_print_error(stack_error_t err)
{
    printf("Error code %d: ", err);
    switch(err)
    {
        case STACK_OK:
            printf("OK\n");
            break;
        case STACK_EMPTY:
            printf("EMPTY\n");
            break;
        case STACK_BAD_ALLOC:
            printf("BAD_ALLOC\n");
            break;
        case STACK_NULL_POINTER:
            printf("STACK_NULL_POINTER\n");
            break;
        case STACK_ALREADY_EXIST:
            printf("ALREADY_EXIST\n");
            break;
        case STACK_BAD_SIZE:
            printf("BAD_SIZE\n");
            break;
        case STACK_BAD_DATA:
            printf("BAD_DATA\n");
            break;
        case STACK_BAD_CAPACITY:
            printf("BAD_CAPACITY\n");
            break;
        case STACK_NEGATIVE_SIZE:
            printf("NEGATIVE_SIZE\n");
            break;
        case STACK_NEGATIVE_CAPACITY:
            printf("NEGATIVE_CAPACITY\n");
            break;
        case STACK_POISON_IN_DATA:
            printf("POISON_IN_DATA\n");
    }
}

stack_error_t stack_canary(stack_t* stk)
{
    ON_CANARY(
        if (stk->canary_left != STACK_CANARY_LEFT) {
            stk->error = STACK_CANARY_LEFT_ERR;
            stack_dump(stk);
        } else if (stk->canary_right != STACK_CANARY_RIGHT) {
            stk->error = STACK_CANARY_RIGHT_ERR;
            stack_dump(stk);
        } else if (*(stk->buffer_canary_left) != BUFFER_CANARY_LEFT) {
            stk->error = BUFFER_CANARY_LEFT_ERR;
            stack_dump(stk);
        } else if (*(stk->buffer_canary_right) != BUFFER_CANARY_RIGHT) {
            stk->error = BUFFER_CANARY_RIGHT_ERR;
            stack_dump(stk);
        }

        return stk->error;
    )
    
    return STACK_OK;
}