#ifndef COMMANDS_H
#define COMMANDS_H

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
    POP = 22,
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
} cmd_struct_t;

typedef struct {
    const char* reg_name;
    reg_t reg_val;
} reg_struct_t;

const cmd_struct_t commands[] = {
    {"PUSH", PUSH, 4},
    {"ADD",  ADD,  3},
    {"SUB",  SUB,  3},
    {"DIV",  DIV,  3},
    {"MUL",  MUL,  3},
    {"OUT",  OUT,  3},
    {"HLT",  HLT,  3},
    {"PUSHR", PUSHR, 5},
    {"POPR",  POPR,  5},
    {"JMP",  JMP,    3},
    {"JA",   JA,     2},
    {"JAE",  JAE,    3},
    {"JB",   JB,     2},
    {"JBE",  JBE,    3},
    {"JE",   JE,     2},
    {"JNE",  JNE,    3},
    {"IN",   IN,     2},
    {"CALL", CALL,   4},
    {"RET",  RET,    3},
    {"PUSHM", PUSHM, 5},
    {"POPM", POPM,   4},
    {"POP", POP,     3}
};

size_t REGS_CNT = 4;

const reg_struct_t registers[] = {
    {"AX", AX},
    {"BX", BX},
    {"CX", CX},
    {"DX", DX}
};

#endif