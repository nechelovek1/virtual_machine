#include <stdlib.h>

#include "commands.h"

//TODO добавить функцию парсер для комманды
const cmd_struct_t commands[CMD_CNT] = {
    {"HLT",   HLT,   sizeof("HLT") - 1,   &cmd_hlt},
    {"PUSH",  PUSH,  sizeof("PUSH") - 1,  &cmd_push_new},
    {"POP",   POP,   sizeof("POP") - 1,   &cmd_pop_new},
    {"ADD",   ADD,   sizeof("ADD") - 1,   &cmd_add},
    {"SUB",   SUB,   sizeof("SUB") - 1,   &cmd_sub},
    {"DIV",   DIV,   sizeof("DIV") - 1,   &cmd_div},
    {"MUL",   MUL,   sizeof("MUL") - 1,   &cmd_mul},
    {"OUT",   OUT,   sizeof("OUT") - 1,   &cmd_out},
    {"JMP",   JMP,   sizeof("JMP") - 1,   &cmd_jmp},
    {"JA",    JA,    sizeof("JA") - 1,    &cmd_ja},
    {"JAE",   JAE,   sizeof("JAE") - 1,   &cmd_jae},
    {"JB",    JB,    sizeof("JB") - 1,    &cmd_jb},
    {"JBE",   JBE,   sizeof("JBE") - 1,   &cmd_jbe},
    {"JE",    JE,    sizeof("JE") - 1,    &cmd_je},
    {"JNE",   JNE,   sizeof("JNE") - 1,   &cmd_jne},
    {"IN",    IN,    sizeof("IN") - 1,    &cmd_in},
    {"CALL",  CALL,  sizeof("CALL") - 1,  &cmd_call},
    {"RET",   RET,   sizeof("RET") - 1,   &cmd_ret},
    {"SQRT",  SQRT,  sizeof("SQRT") - 1,  &cmd_sqrt},
    {"PRNT",  PRNT,  sizeof("PRNT") - 1,  &cmd_print}
};

const reg_struct_t registers[REGS_CNT] = {
    {"AX", REG_AX},
    {"BX", REG_BX},
    {"CX", REG_CX},
    {"DX", REG_DX},
    {"EX", REG_EX},
    {"FX", REG_FX},
    {"GX", REG_GX},
    {"HX", REG_HX},
};