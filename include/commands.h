#ifndef COMMANDS_H
#define COMMANDS_H

#include "stack.h"

typedef enum {
    HLT,
    PUSH,
    POP,
    ADD,
    SUB,
    DIV,
    MUL,
    OUT,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,
    IN,
    CALL,
    RET,
    SQRT,
    PRNT,
    CMD_CNT
} command_t;

typedef enum {
    REG_AX,
    REG_BX,
    REG_CX,
    REG_DX,
    REG_EX,
    REG_FX,
    REG_GX,
    REG_HX,
    REGS_CNT
} reg_t;

typedef struct _proc{
    stack_t stk;
    stack_elem_t* code;
    int code_size;
    stack_elem_t regs[REGS_CNT];
    stack_t call_stk;
    stack_elem_t *ram;
    int vram_height;
    int vram_width;
    int ram_size;
    int pc;
} processor_t;

typedef enum {
    PROCESSOR_OK,
    PROCESSOR_STACK_UNDERFLOW,
    PROCESSOR_CALL_STACK_UNDERFLOW,
    PROCESSOR_DIVISION_BY_ZERO,
    PROCESSOR_OUT_OF_CODE,
    PROCESSOR_UNKNOWN_REG,
    PROCESSOR_OUT_OF_MEMORY,
    PROCESSOR_BAD_BIT_FIELDS
} processor_err_t;

typedef struct {
    const char* cmd_word;
    command_t cmd_code;
    size_t cmd_word_len;
    processor_err_t (*cmd_func) (processor_t*);
} cmd_struct_t;

typedef struct {
    const char* reg_name;
    reg_t reg_val;
} reg_struct_t;

const size_t MAX_CMD_LEN = 5;
const size_t MAX_REG_LEN = 2;
const size_t NUMS_PER_LINE = 3;
const size_t MAX_LABEL_LEN = 128;
const size_t FLT_MULTIPLIER = 100;
 
extern const cmd_struct_t commands[];
extern const reg_struct_t registers[];

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
processor_err_t cmd_push_new(processor_t* proc);
processor_err_t cmd_pop_new(processor_t* proc);
processor_err_t cmd_print(processor_t* proc);


#endif