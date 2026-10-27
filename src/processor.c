#include <stdio.h>
#include <stdbool.h>

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

    for (size_t i = 0; i < CMD_CNT; i++) {
        proc->commands[commands[i].cmd_code] = commands[i].cmd_func;
    }

    return 0;
}

int processor_loop(processor_t* proc)
{
    while (true) {
        if (proc->pc > proc->code_size || proc->pc < 0) {
            printf("Segmentation fault! Invalide PC: %d\n", proc->pc);
            return -1;
        }

        int cmd = proc->code[proc->pc];
        if (cmd == HLT) {
            return 0;
        }

        if (cmd < 0 || cmd >= MAX_CMD_NUM || proc->commands[cmd] == NULL) {
            printf("Segmantation fault! Unknown cmd: %d\n", cmd);
            printf("PC: %d\n", proc->pc);
            return -1;
        }

        if (proc->commands[cmd](proc)) {
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

int cmd_push(processor_t* proc)
{
    proc->pc++;
    stack_push(&(proc->stk), proc->code[proc->pc]);
    proc->pc++;
    return 0;
}

int cmd_pop(processor_t* proc)
{
    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return -1;
    }   

    proc->pc++;
    
    return 0;
}

int cmd_add(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    c = a + b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return 0;
}

int cmd_sub(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    c = a - b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return 0;
}

int cmd_div(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    c = a / b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return 0;
}

int cmd_mul(processor_t* proc)
{
    stack_elem_t a = 0, b = 0, c = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    c = a * b;
    stack_push(&(proc->stk), c);

    proc->pc++;
    
    return 0;  
}

int cmd_out(processor_t* proc)
{
    stack_elem_t a = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    printf("%d\n", a);

    proc->pc++;
    
    return 0;
}

int cmd_hlt(processor_t* proc)
{
    return 0;
}

int cmd_push_r(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }

    stack_elem_t reg = proc->code[proc->pc];
    if (reg > REGS_CNT || reg < 0) {
        return -2;
    }

    stack_push(&(proc->stk), proc->regs[reg]);
    proc->pc++;

    return 0;
}

int cmd_pop_r(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   
    
    stack_elem_t reg = proc->code[proc->pc];
    if (reg > REGS_CNT || reg < 0) {
        return -2;
    }

    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return -1;
    }

    proc->regs[reg] = elem;
    proc->pc++;

    return 0;
}

int cmd_jmp(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    proc->pc = proc->code[proc->pc];

    return 0;
}

int cmd_ja(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a > b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;
}

int cmd_jae(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a >= b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;
}

int cmd_jb(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a < b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;
}

int cmd_jbe(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a >= b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;

}

int cmd_je(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a == b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;
}

int cmd_jne(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }   

    stack_elem_t a = 0, b = 0;

    if (stack_pop(&(proc->stk), &a) == STACK_EMPTY) {
        return -1;
    }
    if (stack_pop(&(proc->stk), &b) == STACK_EMPTY) {
        return -1;
    }   

    if (a != b) {
        proc->pc = proc->code[proc->pc];
    } else {
        proc->pc++;
    }

    return 0;
}

int cmd_in(processor_t* proc)
{
    proc->pc++;

    stack_elem_t elem = 0;
    scanf("%d", &elem);

    stack_push(&(proc->stk), elem);

    return 0;
}

int cmd_call(processor_t* proc)
{
    proc->pc++;
    if (proc->pc >= proc->code_size) {
        return -1;
    }

    stack_push(&(proc->call_stk), proc->pc + 1);

    proc->pc = proc->code[proc->pc];

    return 0;
}

int cmd_ret(processor_t* proc)
{
    if (stack_pop(&(proc->call_stk), &(proc->pc)) == STACK_EMPTY) {
        return -2;
    }

    return 0;
}

int processor_dump(processor_t* proc)
{

    return 0;
}