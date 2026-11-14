#include <stdio.h>

#include "processor.h"
#include "stack.h"
#include "text.h"
#include "commands.h"


int processor_read_bytecode(processor_t* proc, FILE* fp)
{
    size_t bytecode_size = getFileSize(fp) / sizeof(stack_elem_t);
    proc->code_size = bytecode_size;
    proc->code = (stack_elem_t*) calloc(bytecode_size, sizeof(stack_elem_t));
    fread(proc->code, sizeof(stack_elem_t), bytecode_size, fp);

    return 0;
}

int processor_init(processor_t* proc, int height, int width)
{
    proc->stk = (stack_t) {};
    stack_init(&(proc->stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "stk"));

    proc->call_stk = (stack_t) {};
    stack_init(&(proc->call_stk), STACK_MIN_CAPACITY ON_DBG(, __FILE__, __func__, __LINE__, "call_stk"));

    proc->vram_height = height;
    proc->vram_width = width;

    proc->ram_size = proc->vram_height * proc->vram_width;
    proc->ram = (stack_elem_t* ) calloc(proc->ram_size, sizeof(stack_elem_t));
    
    if (proc->ram == NULL) {
        processor_destroy(proc);
        return -1;
    }

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
        processor_err_t res = PROCESSOR_OK;
        if ((res = commands[cmd].cmd_func(proc)) != PROCESSOR_OK) {
            printf("Segmentation fault! Failed to run command: %d\n", cmd);
            printf("Error: %d\n", res);
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

    if (proc->code != NULL) {
        free(proc->code);
    } 

    if (proc->ram != NULL) {
        free(proc->ram);
    }

    *proc = (processor_t) {};

    return 0;
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

    int cmd = proc->code[proc->pc] & 0x1f;
    if (cmd >= 0 && cmd < CMD_CNT) {
        fprintf(stderr, "Current command: %d - %s\n", commands[cmd].cmd_code, commands[cmd].cmd_word);
    } else {
        fprintf(stderr, "Current command: %d - Unknown command\n", cmd);
    }
}