#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdlib.h>

#include "stack.h"

extern const size_t MAX_CMD_NUM;
extern const size_t STACK_MIN_CAPACITY;

typedef struct _proc{
    stack_t stk;
    stack_elem_t* code;
    size_t code_size;
    stack_elem_t regs[4];
    stack_t call_stk;
    int pc;
} processor_t;

typedef enum {
    PROCESSOR_OK,
    PROCESSOR_STACK_UNDERFLOW,
    PROCESSOR_CALL_STACK_UNDERFLOW,
    PROCESSOR_DIVISION_BY_ZERO,
    PROCESSOR_SEGMENTATION_FAULT,
    PROCESSOR_UNKNOWN_REG,
} processor_err_t;

int processor_read_bytecode(processor_t* proc, FILE* fp);
int processor_init(processor_t* proc);
int processor_loop(processor_t* proc);
int processor_destroy(processor_t* proc);
void processor_dump(processor_t* proc);

processor_err_t cmd_push(processor_t* proc);
processor_err_t cmd_pop(processor_t* proc);
processor_err_t cmd_add(processor_t* proc);
processor_err_t cmd_sub(processor_t* proc);
processor_err_t cmd_div(processor_t* proc);
processor_err_t cmd_mul(processor_t* proc);
processor_err_t cmd_out(processor_t* proc);
processor_err_t cmd_hlt(processor_t* proc);
processor_err_t cmd_push_r(processor_t* proc);
processor_err_t cmd_pop_r(processor_t* proc);
processor_err_t cmd_jmp(processor_t* proc);
processor_err_t cmd_ja(processor_t* proc);
processor_err_t cmd_jae(processor_t* proc);
processor_err_t cmd_jb(processor_t* proc);
processor_err_t cmd_jbe(processor_t* proc);
processor_err_t cmd_je(processor_t* proc);
processor_err_t cmd_jne(processor_t* proc);
processor_err_t cmd_in(processor_t* proc);
processor_err_t cmd_call(processor_t* proc);
processor_err_t cmd_ret(processor_t* proc);
processor_err_t cmd_sqrt(processor_t* proc);

#endif