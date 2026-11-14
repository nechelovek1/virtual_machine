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

processor_err_t cmd_push(processor_t* proc)
{
    proc->pc++;
    stack_push(&(proc->stk), proc->code[proc->pc]);
    proc->pc++;
    return PROCESSOR_OK;
}

processor_err_t cmd_pop(processor_t* proc)
{
    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_add(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    c = a + b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_sub(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    c = a - b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_div(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }

    if (b == 0) {
        return PROCESSOR_DIVISION_BY_ZERO;
    }

    c = a / b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_mul(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    c = a * b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return PROCESSOR_OK;  
}

processor_err_t cmd_out(processor_t* proc)
{
    stack_elem_t a = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    printf("%d\n", a);

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_hlt(processor_t* proc)
{
    printf("HALTED PC: %d\n", proc->pc);
    return PROCESSOR_OK;
}

processor_err_t cmd_push_r(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }

    stack_elem_t reg = proc->code[proc->pc];
    if (reg > REGS_CNT || reg < 0) {
        return PROCESSOR_UNKNOWN_REG;
    }

    stack_push(&(proc->stk), proc->regs[reg]);
    proc->pc++;

    return PROCESSOR_OK;
}

processor_err_t cmd_pop_r(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   
    
    stack_elem_t reg = proc->code[proc->pc];
    if (reg > REGS_CNT || reg < 0) {
        return PROCESSOR_UNKNOWN_REG;
    }

    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }

    proc->regs[reg] = elem;
    proc->pc++;

    return PROCESSOR_OK;
}

processor_err_t cmd_jmp(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    proc->pc = proc->code[proc->pc];

    return PROCESSOR_OK;
}

processor_err_t cmd_ja(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a > b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_jae(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a >= b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_jb(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a < b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_jbe(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a <= b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_je(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a == b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_jne(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }   

    if (a != b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_in(processor_t* proc)
{
    proc->pc++;

    stack_elem_t elem = 0;
    scanf("%d", &elem);

    stack_push(&(proc->stk), elem);

    return PROCESSOR_OK;
}

processor_err_t cmd_call(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_OUT_OF_CODE;
    }

    stack_push(&(proc->call_stk), proc->pc + 1);

    proc->pc = proc->code[proc->pc];

    return PROCESSOR_OK;
}

processor_err_t cmd_ret(processor_t* proc)
{
    if (stack_pop(&(proc->call_stk), &(proc->pc)) == STACK_EMPTY) {
        return PROCESSOR_CALL_STACK_UNDERFLOW;
    }

    return PROCESSOR_OK;
}

processor_err_t cmd_sqrt(processor_t* proc)
{
    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return PROCESSOR_STACK_UNDERFLOW;
    }

    elem = (int) ((sqrt((double) elem / FLT_MULTIPLIER)) * FLT_MULTIPLIER);
    stack_push(&(proc->stk), elem);

    proc->pc++;
    
    return PROCESSOR_OK;
}

processor_err_t cmd_print(processor_t* proc)
{
    proc->pc++;

    for (int i = 0; i < proc->vram_height; i++) {
        for (int j = 0; j < proc->vram_width; j++) {
            if (proc->ram[i * proc->vram_width + j] == 1) {
                putchar('#');
                putchar('#');
            } else {
                putchar(' ');
                putchar(' ');
            }
        }
        putchar('\n');
    }
        
    return PROCESSOR_OK;
}

processor_err_t cmd_push_new(processor_t* proc)
{
    stack_elem_t cmd = proc->code[proc->pc];
    proc->pc++;

    stack_elem_t res = 0;
    if (cmd & 0x20) {
        res += proc->code[proc->pc];
        proc->pc++;
    }

    if (cmd & 0x40) {
        stack_elem_t reg = proc->code[proc->pc]; 
        proc->pc++;

        if (reg > REGS_CNT || reg < 0) {
            return PROCESSOR_UNKNOWN_REG;
        }

        res += proc->regs[reg];
    }

    if (cmd & 0x80) {
        if (res > proc->ram_size || res < 0) {
            return PROCESSOR_OUT_OF_MEMORY;
        }

        res = proc->ram[res];
    }

    stack_push(&(proc->stk), res);

    return PROCESSOR_OK;
}

processor_err_t cmd_pop_new(processor_t* proc)
{
    stack_elem_t cmd = proc->code[proc->pc];
    proc->pc++;

    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem)) {
        return PROCESSOR_STACK_UNDERFLOW;
    }

    int addr = 0;
    if (cmd & 0x80) {
        if (cmd & 0x20) {
            addr += proc->code[proc->pc];
            proc->pc++;
        }

        if (cmd & 0x40) {
            stack_elem_t reg = proc->code[proc->pc]; 
            proc->pc++;

            if (reg > REGS_CNT || reg < 0) {
                return PROCESSOR_UNKNOWN_REG;
            }

            addr += proc->regs[reg];
        }

        if (addr > proc->ram_size || addr < 0) {
            return PROCESSOR_OUT_OF_MEMORY;
        }

        proc->ram[addr] = elem;
    } else if ((cmd & 0x40) && !(cmd & 0x20)) {
        stack_elem_t reg = proc->code[proc->pc]; 
        proc->pc++;

        if (reg > REGS_CNT || reg < 0) {
            return PROCESSOR_UNKNOWN_REG;
        }

        proc->regs[reg] = elem;
    } else if ((cmd & 0x40) && (cmd & 0x20)) {
        return PROCESSOR_BAD_BIT_FIELDS;
    } 

    return PROCESSOR_OK;
}
