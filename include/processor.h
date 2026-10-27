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
    int (*commands[256]) (struct _proc*);
} processor_t;

int processor_read_bytecode(processor_t* proc, FILE* fp);
int processor_init(processor_t* proc);
int processor_loop(processor_t* proc);
int processor_destroy(processor_t* proc);
int processor_dump(processor_t* proc);

int cmd_push(processor_t* proc);
int cmd_pop(processor_t* proc);
int cmd_add(processor_t* proc);
int cmd_sub(processor_t* proc);
int cmd_div(processor_t* proc);
int cmd_mul(processor_t* proc);
int cmd_out(processor_t* proc);
int cmd_hlt(processor_t* proc);
int cmd_push_r(processor_t* proc);
int cmd_pop_r(processor_t* proc);
int cmd_jmp(processor_t* proc);
int cmd_ja(processor_t* proc);
int cmd_jae(processor_t* proc);
int cmd_jb(processor_t* proc);
int cmd_jbe(processor_t* proc);
int cmd_je(processor_t* proc);
int cmd_jne(processor_t* proc);
int cmd_in(processor_t* proc);
int cmd_call(processor_t* proc);
int cmd_ret(processor_t* proc);


#endif