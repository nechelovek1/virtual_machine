#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "processor.h"
#include "stack.h"
#include "text.h"
#include "commands.h"

const size_t MAX_CMD_NUM = 0xff;
const size_t STACK_MIN_CAPACITY = 5;

int processor_read_bytecode(processor_t* proc, FILE* fp)
{
    size_t bytecode_size = getFileSize(fp) / sizeof(stack_elem_t);
    proc->code_size = bytecode_size;
    proc->code = (stack_elem_t*) calloc(bytecode_size, sizeof(stack_elem_t));
    fread(proc->code, sizeof(stack_elem_t), bytecode_size, fp);

    return 0;
}

int processor_init(processor_t* proc)
{
    proc->stk = (stack_t) {};
    stack_init(&(proc->stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "stk"));

    proc->call_stk = (stack_t) {};
    stack_init(&(proc->call_stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "call_stk"));

    return 0;
}

int processor_loop(processor_t* proc)
{
    int cmd = CMD_CNT;
    while (cmd != HLT) {
        if (proc->pc > proc->code_size || proc->pc < 0) {
            printf("Segmentation fault! Invalide PC: %d\n", proc->pc);
            return -1;
        }

        cmd = proc->code[proc->pc] & 0x1f;

        if (cmd < 0 || cmd >= CMD_CNT || commands[cmd].cmd_func  == NULL) {
            printf("Segmantation fault! Unknown cmd: %d\n", cmd);
            printf("PC: %d\n", proc->pc);
            return -1;
        }

        //processor_dump(proc);
        //getchar();

        if (commands[cmd].cmd_func(proc)) {
            printf("Segmentation fault! Failed to run command: %d\n", cmd);
            printf("PC: %d\n", proc->pc);
            return -1;
        }

    }

    return 0;
}

int processor_destroy(processor_t* proc)
{
    stack_destroy(&proc->stk);
    stack_destroy(&proc->call_stk);
    free(proc->code);

    return 0;
}

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
    printf("HALTED PC: %d", proc->pc);
    return PROCESSOR_OK;
}

processor_err_t cmd_push_r(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
    }   

    proc->pc = proc->code[proc->pc];

    return PROCESSOR_OK;
}

processor_err_t cmd_ja(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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

processor_err_t cmd_je(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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
        return PROCESSOR_SEGMENTATION_FAULT;
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

void processor_dump(processor_t* proc)
{
    const int PROC_CODE_DUMP_SIZE = 7;
    fprintf(stderr, "Processor stack:\n");
    stack_dump(&(proc->stk));
    fprintf(stderr, "Processor call stack:\n");
    stack_dump(&(proc->call_stk));
    fprintf(stderr, "Processor registers:\n");

    for (int i = 0; i < REGS_CNT; i++) {
        fprintf(stderr, "%-7s ", registers[i].reg_name);
    }
    fputc('\n', stderr);

    for (int i = 0; i < REGS_CNT; i++) {
        fprintf(stderr, "%-7d ", proc->regs[i]);
    }
    fputc('\n', stderr);
    fprintf(stderr, "Processor code segment\n");

    for (int i = proc->pc - PROC_CODE_DUMP_SIZE; i < proc->pc + PROC_CODE_DUMP_SIZE; i++) {
        if (i >= 0) {
            fprintf(stderr, "%-7d ", i);
        }
    }
    fputc('\n', stderr);

    for (int i = proc->pc - PROC_CODE_DUMP_SIZE; i < proc->pc + PROC_CODE_DUMP_SIZE; i++) {
        if (i >= 0 && i < proc->code_size) {
            fprintf(stderr, "%-7d ", proc->code[i]);
        }
    }
    fputc('\n', stderr);

    for (int i = proc->pc - PROC_CODE_DUMP_SIZE; i < proc->pc + PROC_CODE_DUMP_SIZE; i++) {
        if (i >= 0 && i == proc->pc && i < proc->code_size) {
            fprintf(stderr, "%-7s ", "^");
        } else if (i >= 0 && i < proc->code_size) {
            fprintf(stderr, "%-7s ", " ");
        }
    }
    fputc('\n', stderr);

    int cmd = proc->code[proc->pc];
    if (cmd >= 0 && cmd < CMD_CNT) {
        fprintf(stderr, "Current command: %d - %s\n", commands[cmd].cmd_code, commands[cmd].cmd_word);
    } else {
        fprintf(stderr, "Current command: %d - Unknown command\n", cmd);
    }
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
    } else if (cmd & 0x40 && !(cmd & 0x20)) {
        stack_elem_t reg = proc->code[proc->pc]; 
        proc->pc++;

        if (reg > REGS_CNT || reg < 0) {
            return PROCESSOR_UNKNOWN_REG;
        }

        proc->regs[reg] = elem;
    } else if (!(cmd & 0x40) && cmd & 0x20) {
        return PROCESSOR_OK;
    } 

    return PROCESSOR_SEGMENTATION_FAULT; //TODO добавить команду с некорректными опциями
}