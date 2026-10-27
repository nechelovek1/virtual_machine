#include <stdlib.h>

#include "commands.h"

const size_t CMD_CNT = 22;
const size_t REGS_CNT = 4;
const size_t MAX_CMD_LEN = 5;
const size_t NUMS_PER_LINE = 3;

//TODO добавить функцию парсер для комманды
const cmd_struct_t commands[] = {
    {"PUSH",  PUSH,  sizeof("PUSH"),  &cmd_push},
    {"ADD",   ADD,   sizeof("ADD"),   &cmd_add},
    {"SUB",   SUB,   sizeof("SUB"),   &cmd_sub},
    {"DIV",   DIV,   sizeof("DIV"),   &cmd_div},
    {"MUL",   MUL,   sizeof("MUL"),   &cmd_mul},
    {"OUT",   OUT,   sizeof("OUT"),   &cmd_out},
    {"HLT",   HLT,   sizeof("HLT"),   &cmd_hlt},
    {"PUSHR", PUSHR, sizeof("PUSHR"), &cmd_push_r},
    {"POPR",  POPR,  sizeof("POPR"),  &cmd_pop_r},
    {"JMP",   JMP,   sizeof("JMP"),   &cmd_jmp},
    {"JA",    JA,    sizeof("JA"),    &cmd_ja},
    {"JAE",   JAE,   sizeof("JAE"),   &cmd_jae},
    {"JB",    JB,    sizeof("JB"),    &cmd_jb},
    {"JBE",   JBE,   sizeof("JBE"),   &cmd_jbe},
    {"JE",    JE,    sizeof("JE"),    &cmd_je},
    {"JNE",   JNE,   sizeof("JNE"),   &cmd_jne},
    {"IN",    IN,    sizeof("IN"),    &cmd_in},
    {"CALL",  CALL,  sizeof("CALL"),  &cmd_call},
    {"RET",   RET,   sizeof("RET"),   &cmd_ret},
    {"PUSHM", PUSHM, sizeof("PUSHM"), NULL},
    {"POPM",  POPM,  sizeof("POPM"),  NULL},
    {"POP",   POP,   sizeof("POP"),   &cmd_pop}
};

const reg_struct_t registers[] = {
    {"AX", AX},
    {"BX", BX},
    {"CX", CX},
    {"DX", DX}
};