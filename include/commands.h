#ifndef COMMANDS_H
#define COMMANDS_H

#include "processor.h"

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

typedef struct {
    const char* cmd_word;
    command_t cmd_code;
    size_t cmd_word_len;
    processor_err_t (*cmd_func) (struct _proc*);
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

#endif