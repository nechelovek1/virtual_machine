#include <stdlib.h>

#include "commands.h"

const size_t CMD_CNT = 22;
const size_t REGS_CNT = 4;

const cmd_struct_t commands[] = {
    {"PUSH",  PUSH,  4, &cmd_push},
    {"ADD",   ADD,   3, &cmd_add},
    {"SUB",   SUB,   3, &cmd_sub},
    {"DIV",   DIV,   3, &cmd_div},
    {"MUL",   MUL,   3, &cmd_mul},
    {"OUT",   OUT,   3, &cmd_out},
    {"HLT",   HLT,   3, &cmd_hlt},
    {"PUSHR", PUSHR, 5, &cmd_push_r},
    {"POPR",  POPR,  5, &cmd_pop_r},
    {"JMP",   JMP,   3, &cmd_jmp},
    {"JA",    JA,    2, &cmd_ja},
    {"JAE",   JAE,   3, &cmd_jae},
    {"JB",    JB,    2, &cmd_jb},
    {"JBE",   JBE,   3, &cmd_jbe},
    {"JE",    JE,    2, &cmd_je},
    {"JNE",   JNE,   3, &cmd_jne},
    {"IN",    IN,    2, &cmd_in},
    {"CALL",  CALL,  4, &cmd_call},
    {"RET",   RET,   3, &cmd_ret},
    {"PUSHM", PUSHM, 5, NULL},
    {"POPM",  POPM,  4, NULL},
    {"POP",   POP,   3, &cmd_pop}
};

const reg_struct_t registers[] = {
    {"AX", AX},
    {"BX", BX},
    {"CX", CX},
    {"DX", DX}
};