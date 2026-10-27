#ifndef COMMANDS_H
#define COMMANDS_H

#include "processor.h"

typedef enum {
    PUSH  = 1,
    ADD   = 2,
    SUB   = 3,
    DIV   = 4,
    MUL   = 5,
    OUT   = 6,
    HLT   = 7,
    PUSHR = 8,
    POPR  = 9,
    JMP   = 10,
    JA    = 11,
    JAE   = 12,
    JB    = 13,
    JBE   = 14,
    JE    = 15,
    JNE   = 16,
    IN    = 17,
    CALL  = 18,
    RET   = 19,
    PUSHM = 20,
    POPM  = 21,
    POP   = 22,
} command_t;

typedef enum {
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3
} reg_t;

typedef struct {
    const char* cmd_word;
    command_t cmd_code;
    size_t cmd_word_len;
    int (*cmd_func) (processor_t*);
} cmd_struct_t;

typedef struct {
    const char* reg_name;
    reg_t reg_val;
} reg_struct_t;

extern const size_t CMD_CNT;
extern const size_t REGS_CNT;
extern const size_t MAX_CMD_LEN;
extern const size_t NUMS_PER_LINE;

extern const cmd_struct_t commands[];
extern const reg_struct_t registers[];


#endif