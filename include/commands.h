#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum  {
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
} command_t;

#endif