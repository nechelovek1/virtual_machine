#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "text.h"
#include "stack.h"
#include "commands.h"
#include "assembler.h"

int translate_context_read_text(translate_ctx_t* ctx, FILE* fp)
{
    ctx->text = (textfile_t) {};
    read_text(fp, &(ctx->text));
    
    ctx->bytecode = (int*) calloc(ctx->text.nLines * 3, sizeof(int));
    ctx->labels = (label_t*) calloc(ctx->text.nLines, sizeof(label_t));

    ctx->labels_cnt = 0;
    ctx->bytecode_size = 0;

    return 0;
}

cmd_read_err_t translate_text(translate_ctx_t* ctx)
{
    for (size_t i = 0; i < ctx->text.nLines; i++)
    {
        line_t cmd_line = ctx->text.lines[i];
        if (cmd_line.len == 0) {
            continue;
        }
        
        cmd_read_err_t cmd_res = read_cmd(ctx, cmd_line);

        if (cmd_res == TRANSLATE_OK) {
            continue;
        } else if (!read_label(ctx, cmd_line, ctx->bytecode_size)) {
            continue;
        } else {
            printf("Error %d on line %ld\n", cmd_res, i + 1);
            return cmd_res;
        }
    }

    return TRANSLATE_OK;
}

cmd_read_err_t read_cmd(translate_ctx_t* ctx, line_t cmd_line)
{
    size_t cmd_len = 0;
    int elem = 0, n = 0, res = 0;
    command_t cmd = PUSHM;
    cmd_read_err_t read_res = read_command_word(cmd_line.line, &cmd_len, &cmd);

    if (read_res != TRANSLATE_OK) {
        return read_res;
    }

    //TODO избавиться от копипасты (сделать новую функцию)

    switch(cmd)
    {
        case PUSH:
            ctx->bytecode[ctx->bytecode_size] = PUSH;
            ctx->bytecode_size++;

            res = sscanf(cmd_line.line + cmd_len, "%d%n", &elem, &n);
            cmd_len += n;
            if (res != 1) {
                return TRANSLATE_MISSING_ARGS;
            }

            ctx->bytecode[ctx->bytecode_size] = elem;
            ctx->bytecode_size++;
            
            return TRANSLATE_OK;
        case PUSHR:
            ctx->bytecode[ctx->bytecode_size] = PUSHR;
            ctx->bytecode_size++;
            if (read_register(&ctx->bytecode[ctx->bytecode_size], cmd_line, &cmd_len)) {
                return TRANSLATE_UNKNOWN_REG;
            }
            ctx->bytecode_size++;
            
           return TRANSLATE_OK;
        case POPR:
            ctx->bytecode[ctx->bytecode_size] = POPR;
            ctx->bytecode_size++;
            if (read_register(&ctx->bytecode[ctx->bytecode_size], cmd_line, &cmd_len)) {
                return TRANSLATE_UNKNOWN_REG;
            }
            ctx->bytecode_size++;
            
            return TRANSLATE_OK;
        case PUSHM:
            return TRANSLATE_UNKNOWN_CMD;
        case POPM:
            return TRANSLATE_UNKNOWN_CMD;
        case POP:
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case IN:
        case OUT:
        case RET:
        case HLT:
            ctx->bytecode[ctx->bytecode_size] = cmd;
            ctx->bytecode_size++;
            return TRANSLATE_OK;
        case JMP:
        case JA:
        case JAE:
        case JB:
        case JBE:
        case JE:
        case JNE:
        case CALL:
            return read_jump(ctx, JNE, cmd_line, cmd_len);
        default:
            return TRANSLATE_UNKNOWN_CMD;
    }

    return TRANSLATE_OK;
}

int create_label(translate_ctx_t* ctx, int label_value, int label_address)
{
    ctx->labels[ctx->labels_cnt] = (label_t) {label_value, label_address};
    ctx->labels_cnt++;    
}

int find_label(translate_ctx_t* ctx, int label_value)
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) {
        if (ctx->labels[i].value == label_value) {
            return i;
        }   
    }

    return -1;
}

cmd_read_err_t read_label(translate_ctx_t* ctx, line_t label_line, int label_address)
{
    if (!ctx->mode) {
        return TRANSLATE_OK;
    }

    int label_value = 0, n = 0;
    int res = sscanf(label_line.line, "%d%n", &label_value, &n);
    
    if (res != 1) {
        return TRANSLATE_MISSING_ARGS;
    }

    if (label_line.line[n] != ':') {
        return TRANSLATE_BAD_LABEL;
    }

    int find_res = find_label(ctx, label_value);

    if (find_res == -1) {
        create_label(ctx, label_value, label_address);
        return TRANSLATE_OK;
    } 

    if (ctx->labels[find_res].address == -1) {
        ctx->labels[find_res].address = label_address;
        return TRANSLATE_OK;
    }

    return TRANSLATE_LABEL_EXIST;
}

cmd_read_err_t read_jump(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    ctx->bytecode[ctx->bytecode_size] = cmd_code;
    ctx->bytecode_size++;

    int label_value = 0, n = 0;
    int res = sscanf(cmd_line.line + cmd_len, "%d%n", &label_value, &n);
    
    if (res != 1) {
        return TRANSLATE_MISSING_ARGS;
    }
    
    cmd_len += n;

    if (cmd_line.line[cmd_len] == ':') {
        int label_idx = find_label(ctx, label_value);
        if (label_idx == -1) {
            if (ctx->mode) {
                create_label(ctx, label_value, -1);
            } else {
                return TRANSLATE_BAD_LABEL;
            }
        } else if (ctx->labels[label_idx].address == -1){
            return TRANSLATE_BAD_LABEL;
        } else {
            ctx->bytecode[ctx->bytecode_size] = ctx->labels[label_idx].address;
        }
    } else {
        ctx->bytecode[ctx->bytecode_size] = label_value;
    }
    ctx->bytecode_size++;

    return TRANSLATE_OK;
}

cmd_read_err_t read_register(int* reg, line_t cmd_line, size_t* cmd_len)
{
    char reg_name[3] = "";
    int n = 0;

    sscanf(cmd_line.line + *cmd_len, "%2s%n", reg_name, &n);
    *cmd_len += n;

    for (size_t i = 0; i < REGS_CNT; i++) {
        if (!strcmp(reg_name, registers[i].reg_name)) {
            *reg = registers[i].reg_val;
            return TRANSLATE_OK;
        }
    }
    
    return TRANSLATE_UNKNOWN_REG;
}

void print_labels(translate_ctx_t* ctx)
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) {
        printf("lbl: %d addr: %d\n", ctx->labels[i].value, ctx->labels[i].address);
    }
}

int translate_context_destroy(translate_ctx_t* ctx)
{
    free(ctx->labels);
    free(ctx->bytecode);
    *ctx = (translate_ctx_t){};
}

cmd_read_err_t read_command_word(const char* line, size_t* cmd_len, command_t* cmd_code)
{
    char cmd_word[6] = "";
    sscanf(line, "%5s", cmd_word);
    
    for (size_t i = 0; i < CMD_CNT; i++) {
        if (!strcmp(cmd_word, commands[i].cmd_word)) {
            *cmd_len = commands[i].cmd_word_len;
            *cmd_code = commands[i].cmd_code;
            return TRANSLATE_OK;
        }
    }

    return TRANSLATE_UNKNOWN_CMD;
}