#include <stdio.h>
#include <stdbool.h>

#include "stack.h"
#include "commands.h"

const size_t REG_CNT = 4;
const size_t MAX_CMD_NUM = 0xff;

typedef struct {
    stack_t stk;
    stack_elem_t* code;
    size_t code_size;
    stack_elem_t regs[REG_CNT];
    stack_t call_stk;
    int pc;
    int (*commands[MAX_CMD_NUM]) (processor_t*);
} processor_t;

const STACK_MIN_CAPACITY = 5;

//stack_elem_t* read_bytecode(FILE* file);

int processor_read_bytecode(processor_t* proc, FILE* fp);
int processor_init(processor_t* proc);
int processor_loop(processor_t* proc);

int cmd_push(processor_t* proc);
int cmd_add(processor_t* proc);
int cmd_sub(processor_t* proc);
int cmd_div(processor_t* proc);
int cmd_mul(processor_t* proc);
int cmd_out(processor_t* proc);
int cmd_hlt(processor_t* proc);
int cmd_push_r(processor_t* proc);
int cmd_pop_r(processor_t* proc);
int cmd_jmp(processor_t* proc);
int cmd_ja(processor_t* proc);
int cmd_jae(processor_t* proc);
int cmd_jb(processor_t* proc);
int cmd_jbe(processor_t* proc);
int cmd_je(processor_t* proc);
int cmd_jne(processor_t* proc);
int cmd_in(processor_t* proc);
int cmd_call(processor_t* proc);
int cmd_ret(processor_t* proc);


//void hlt(stack_t* stk);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <byte code file>\n", argv[0]);
    }

    FILE* file = NULL;
    if ((file = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open %s\n", argv[1]);
        return -1;
    }

    processor_t proc = {};

    processor_init(&proc);
    processor_read_bytecode(&proc, file);
    processor_run(&proc);

    return 0;
}

int processor_read_bytecode(processor_t* proc, FILE* fp)
{
    size_t bytecode_size = getFileSize(fp);
    proc->code_size = bytecode_size;
    proc->code = calloc(bytecode_size, sizeof(stack_elem_t));
    fread(proc->code, sizeof(stack_elem_t), bytecode_size, fp);
}

int processor_init(processor_t* proc)
{
    proc->stk = (stack_t) {};
    stack_init(&(proc->stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "stk"));

    proc->call_stk = (stack_t) {};
    stack_init(&(proc->call_stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "call_stk"));

    proc->commands[PUSH] = &cmd_push;
    proc->commands[ADD] =  &cmd_add;
    proc->commands[SUB] =  &cmd_sub;
    proc->commands[DIV] = &cmd_div;
    proc->commands[MUL] = &cmd_mul;
    proc->commands[OUT] = &cmd_out;
    proc->commands[HLT] = &cmd_hlt;
    proc->commands[PUSHR] = &cmd_push_r;
    proc->commands[POPR] = &cmd_pop_r;
    proc->commands[JMP] = &cmd_jmp;
    proc->commands[JA] = &cmd_ja;
    proc->commands[JAE] = &cmd_jae;
    proc->commands[JB] = &cmd_jb;
    proc->commands[JBE] = &cmd_jbe;
    proc->commands[JE] = &cmd_je;
    proc->commands[JNE] = &cmd_jne;
    proc->commands[IN] = &cmd_in;
    proc->commands[CALL] = &cmd_call;
    proc->commands[RET] = &cmd_ret;
}

int processor_loop(processor_t* proc)
{
    while (proc->pc < proc->code_size) {
        int cmd = proc->code[proc->pc];
        if (cmd < 0 || cmd >= MAX_CMD_NUM) {
            printf("Segmantation fault!\n");
            return -1;
        }

        if (proc->commands[cmd] == NULL) {
            printf("Segmantation fault!\n");
            return -1;            
        }

        if (!proc->commands[cmd](proc)) {
            printf("Segmentation fault!\n");
        }
    }

    return 0;
}

int cmd_push(processor_t* proc)
{
    proc->pc++;
    stack_push(&(proc->stk), proc->code[proc->pc]);
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
    if (reg > REG_CNT || reg < 0) {
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
    if (reg > REG_CNT || reg < 0) {
        return -2;
    }

    stack_elem_t elem = 0;
    if (stack_pop(&(proc->stk), &elem) == STACK_EMPTY) {
        return -1;
    }

    proc->regs[reg] = elem;

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