#ifndef COMMANDS_H
#define COMMANDS_H

#include "processor.h"

typedef enum {
    HLT,
    PUSH,
    ADD,
    SUB,
    DIV,
    MUL,
    OUT,
    PUSHR,
    POPR,
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
    PUSHM,
    POPM,
    POP,
    CMD_CNT
} command_t;

typedef enum {
    AX,
    BX,
    CX,
    DX,
    REGS_CNT
} reg_t;

typedef struct {
    const char* cmd_word;
    command_t cmd_code;
    size_t cmd_word_len;
    int (*cmd_func) (struct _proc*);
} cmd_struct_t;

typedef struct {
    const char* reg_name;
    reg_t reg_val;
} reg_struct_t;

const size_t MAX_CMD_LEN = 5;
const size_t MAX_REG_LEN = 2;
const size_t NUMS_PER_LINE = 3;
const size_t MAX_LABEL_LEN = 128;

extern const cmd_struct_t commands[];
extern const reg_struct_t registers[];


#endif