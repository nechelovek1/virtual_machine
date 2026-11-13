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
    
    ctx->bytecode = (int*) calloc(ctx->text.nLines * NUMS_PER_LINE, sizeof(int));
    ctx->labels = (label_t*) calloc(ctx->text.nLines, sizeof(label_t));

    ctx->labels_cnt = 0;
    ctx->bytecode_size = 0;
    ctx->pc = 0;

    ASM_LOG("[LOG]: Translate context created\n")

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
        
        cmd_read_err_t res = read_cmd(ctx, cmd_line);

        if (res == TRANSLATE_OK) {
            continue;
        } else if (read_label(ctx, cmd_line, ctx->pc) == TRANSLATE_OK) {
            continue;
        } else if (read_coment(cmd_line) == TRANSLATE_OK) {
            continue;
        } else {
            printf("Error %d on line %ld\n", res, i + 1);
            print_assembler_error(res);
            return res;
        }
    }

    return TRANSLATE_OK;
}

cmd_read_err_t read_cmd(translate_ctx_t* ctx, line_t cmd_line)
{
    size_t cmd_len = 0;
    command_t cmd_code = PUSHM;
    cmd_read_err_t read_res = read_command_word(cmd_line.line, &cmd_len, &cmd_code);

    if (read_res != TRANSLATE_OK) {
        return read_res;
    }

    switch(cmd_code)
    {
        case PUSH:
            return read_push(ctx, cmd_code, cmd_line, cmd_len);
        case PUSHR:            
        case POPR:
            return read_pushr_popr(ctx, cmd_code, cmd_line, cmd_len);
        case PUSHM:
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
        case SQRT:
            return read_cmd_without_args(ctx, cmd_code, cmd_line, cmd_len);
        case JMP:
        case JA:
        case JAE:
        case JB:
        case JBE:
        case JE:
        case JNE:
        case CALL:
            return read_jump(ctx, cmd_code, cmd_line, cmd_len);
        default:
            return TRANSLATE_UNKNOWN_CMD;
    }

    return TRANSLATE_OK;
}

int create_label(translate_ctx_t* ctx, const char* label_value, int label_address)
{
    char* label_val_dup = strdup(label_value);
    ctx->labels[ctx->labels_cnt] = (label_t) {label_val_dup, label_address};
    ASM_LOG("[LOG]: Label %s with address %d created\n", label_value, label_address)
    ctx->labels_cnt++;  
    
    return 0;
}

int find_label(translate_ctx_t* ctx, const char* label_value)
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) {
        if (!strcmp(ctx->labels[i].value, label_value)) {
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
    //TODO check label_addres not equal -1

    int n = 0;
    char label_value[MAX_LABEL_LEN + 1] = "";
    int res = sscanf(label_line.line, " %128s%n", label_value, &n);
    
    if (res != 1) {
        return TRANSLATE_MISSING_ARGS;
    }

    if (label_line.line[n - 1] != ':') {
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

cmd_read_err_t read_coment(line_t coment_line)
{
    if (!is_end_valid(coment_line.line, 0)) {
        return TRANSLATE_UNKNOWN_CMD;
    } 
    return TRANSLATE_OK;
}

cmd_read_err_t read_jump(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    ctx->bytecode[ctx->pc] = cmd_code;
    ASM_LOG("[LOG]: Command %d write to %d\n", cmd_code, ctx->pc)
    ctx->pc++;

    int n = 0;
    char label_value[MAX_LABEL_LEN + 1] = "";
    int res = sscanf(cmd_line.line + cmd_len, " %128s%n", label_value, &n);
    
    if (res != 1) {
        return TRANSLATE_MISSING_ARGS;
    }
    
    cmd_len += n;
    //TODO добавить коментарии
    if (cmd_line.line[cmd_len - 1] == ':') {
        int label_idx = find_label(ctx, label_value);
        if (label_idx == -1 && ctx->mode) {
            create_label(ctx, label_value, -1);
            ctx->bytecode[ctx->pc] = -1;

            ASM_LOG("[LOG]: address %d from label %s write to %d\n", -1, label_value, ctx->pc)
        } else if (ctx->mode || (ctx->labels[label_idx].address != -1 && !ctx->mode)){
            ctx->bytecode[ctx->pc] = ctx->labels[label_idx].address;
            
            ASM_LOG("[LOG]: address %d from label %s write to %d\n", ctx->labels[label_idx].address, label_value, ctx->pc)
        } else {
            return TRANSLATE_BAD_LABEL;
        }
    } else {
        //TODO чтение адреса напрямую
        return TRANSLATE_BAD_LABEL;
    }
    ctx->pc++;

    if (!is_end_valid(cmd_line.line, cmd_len)) {
        return TRANSLATE_TOO_MANY_ARGS;
    }

    return TRANSLATE_OK;
}

cmd_read_err_t read_register(int* reg, line_t cmd_line, size_t* cmd_len)
{
    char reg_name[MAX_REG_LEN + 1] = "";
    int n = 0;

    sscanf(cmd_line.line + *cmd_len, "%2s%n", reg_name, &n);
    *cmd_len += n;

    for (size_t i = 0; i < REGS_CNT; i++) {
        if (!strcmp(reg_name, registers[i].reg_name)) {
            ASM_LOG("[LOG]: Found register %s with code %lu\n", reg_name, i)
            *reg = registers[i].reg_val;
            return TRANSLATE_OK;
        }
    }
    
    return TRANSLATE_UNKNOWN_REG;
}

void print_labels(translate_ctx_t* ctx)
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) {
        printf("lbl: %s addr: %d\n", ctx->labels[i].value, ctx->labels[i].address);
    }
}

int translate_context_destroy(translate_ctx_t* ctx)
{
    labels_destroy(ctx);
    free(ctx->labels);
    free(ctx->bytecode);
    freeTextFile(&ctx->text);
    *ctx = (translate_ctx_t){};
    
    ASM_LOG("[LOG]: Aseembler context destroyed\n")

    return 0;
}

cmd_read_err_t read_command_word(const char* line, size_t* cmd_len, command_t* cmd_code)
{
    char cmd_word[MAX_CMD_LEN + 1] = "";
    sscanf(line, "%5s", cmd_word);
    
    for (size_t i = 0; i < CMD_CNT; i++) {
        if (!strcmp(cmd_word, commands[i].cmd_word)) {
            ASM_LOG("[LOG]: Found command %s, code: %d\n", cmd_word, i)
            *cmd_len = commands[i].cmd_word_len;
            *cmd_code = commands[i].cmd_code;
            return TRANSLATE_OK;
        }
    }

    return TRANSLATE_UNKNOWN_CMD;
}

cmd_read_err_t read_push(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    ctx->bytecode[ctx->pc] = cmd_code;
    ASM_LOG("[LOG]: Command %d write to %lu\n", cmd_code, ctx->pc)
    ctx->pc++;

    int elem = 0, n = 0;
    int res = sscanf(cmd_line.line + cmd_len, "%d%n", &elem, &n);
    cmd_len += n;
    
    if (res != 1) {
        return TRANSLATE_MISSING_ARGS;
    }

    ctx->bytecode[ctx->pc] = elem;
    ASM_LOG("[LOG]: Arg %d write to %lu\n", cmd_code, ctx->pc)
    ctx->pc++;

    if (!is_end_valid(cmd_line.line, cmd_len)) {
        return TRANSLATE_TOO_MANY_ARGS;
    }
    
    return TRANSLATE_OK;
}

cmd_read_err_t read_pushr_popr(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    ctx->bytecode[ctx->pc] = cmd_code;
    ASM_LOG("[LOG]: Command %d write to %lu\n", cmd_code, ctx->pc)
    ctx->pc++;
    
    if (read_register(&ctx->bytecode[ctx->pc], cmd_line, &cmd_len)) {
        return TRANSLATE_UNKNOWN_REG;
    }
    ctx->pc++;

    if (!is_end_valid(cmd_line.line, cmd_len)) {
        return TRANSLATE_TOO_MANY_ARGS;
    }
    
    return TRANSLATE_OK;
}

cmd_read_err_t read_cmd_without_args(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    ctx->bytecode[ctx->pc] = cmd_code;
    ASM_LOG("[LOG]: Command %d write to %lu\n", cmd_code, ctx->pc)
    ctx->pc++;

    if (!is_end_valid(cmd_line.line, cmd_len)) {
        return TRANSLATE_TOO_MANY_ARGS;
    }
    
    return TRANSLATE_OK;
}

cmd_read_err_t first_pass(translate_ctx_t* ctx)
{
    ASM_LOG("[LOG]: First pass started\n")

    ctx->mode = true; //Первый проход
    cmd_read_err_t res = translate_text(ctx);
    ctx->bytecode_size = ctx->pc;

    ON_ASM_DBG(
        if (res == TRANSLATE_OK) {
            ASM_LOG("[LOG]: First pass done\n")
        } else {
            ASM_LOG("[LOG]: First pass done with error\n")
            print_assembler_error(res);
        }
    )

    return res;
}

cmd_read_err_t second_pass(translate_ctx_t* ctx)
{
    ASM_LOG("\n\n[LOG]: Second pass started\n")

    ctx->pc = 0;
    ctx->mode = false; //Второй проход
    cmd_read_err_t res = translate_text(ctx);

    ON_ASM_DBG(
        if (res == TRANSLATE_OK) {
            ASM_LOG("[LOG]: Second pass done\n")
        } else {
            ASM_LOG("[LOG]: Second pass done with error\n")
            print_assembler_error(res);
        }
    )

    return res;
}

void labels_destroy(translate_ctx_t* ctx) 
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) 
    {
       free(ctx->labels[i].value); 
    }
}

void assembler_dump(translate_ctx_t* ctx)
{
    print_labels(ctx);
}

void print_assembler_error(cmd_read_err_t err)
{
    switch(err) {
        case TRANSLATE_TOO_MANY_ARGS:
            fprintf(stderr, "[ERROR]: Too many args\n");
            break;
        case TRANSLATE_MISSING_ARGS:
            fprintf(stderr, "[ERROR]: Missing args\n");
            break;
        case TRANSLATE_UNKNOWN_CMD:
            fprintf(stderr, "[ERROR]: Unknown command name\n");
            break;
        case TRANSLATE_UNKNOWN_REG:
            fprintf(stderr, "[ERROR]: Unknown register name\n");
            break;
        case TRANSLATE_BAD_LABEL:
            fprintf(stderr, "[ERROR]: Failed to read label\n");
            break;
        case TRANSLATE_LABEL_EXIST:
            fprintf(stderr, "[ERROR]: Reapeted defenition of label\n");
            break;
        case TRANSLATE_OK:
            fprintf(stderr, "[LOG]: OK\n");
            break;
    }
}

bool is_end_valid(const char* s, int offset)
{
    char c = '\0';
    int res = sscanf(s + offset, " %c", &c);
    return (res == 1 && c == COMENT_CHAR) || (res == -1);
}

cmd_read_err_t read_push_pop(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len)
{
    char reg_name[MAX_REG_LEN + 1] = "";
    int count = 0, reg_val = 0, n = 0;
    char endc = '\0';
    
    int cmd_code_with_bits = cmd_code;

    //TODO сделать "умное чтение"
    if (sscanf(cmd_line.line + cmd_len, "[ %2[A-DX] + %d %c%n", reg_name, &count, &endc, &n) == 3 && endc == ']') {
        cmd_code_with_bits |= 0x80;
        cmd_code_with_bits |= 0x40;
        cmd_code_with_bits |= 0x20;
    } else if (sscanf(cmd_line.line + cmd_len, "[ %2[A-DX] %c%n", reg_name, &endc, &n) == 2 && endc == ']') {
        cmd_code_with_bits |= 0x80;
        cmd_code_with_bits |= 0x40;    
    } else if (sscanf(cmd_line.line + cmd_len, "[ %d %c%n", reg_name, &count, &endc, &n) == 2 && endc == ']') {
        cmd_code_with_bits |= 0x80;
        cmd_code_with_bits |= 0x20;        
    } else if (sscanf(cmd_line.line + cmd_len, " %2[A-DX]%n", reg_name, &n) == 1) {
        cmd_code_with_bits |= 0x40; 
    } else if (sscanf(cmd_line.line + cmd_len, "%d%n", &count, &n) == 1 && cmd_code == PUSH) {
        cmd_code_with_bits |= 0x20;
    } else if (!is_end_valid(cmd_line.line, cmd_len) || cmd_code != POP) {
        return TRANSLATE_TOO_MANY_ARGS;
    }
    
    cmd_len += n;
    if (!is_end_valid(cmd_line.line, cmd_len)) {
        return TRANSLATE_TOO_MANY_ARGS;
    }

    ctx->bytecode[ctx->pc] = cmd_code_with_bits;
    ctx->pc++;

    if (cmd_code_with_bits & 0x20) {
        ctx->bytecode[ctx->pc] = count;
        ctx->pc++;
    }

    if (cmd_code_with_bits & 0x40) {
        if (get_reg_val(cmd_line.line, &reg_val) != TRANSLATE_OK) {
            return TRANSLATE_UNKNOWN_REG;
        }
        ctx->bytecode[ctx->pc] = reg_val;
        ctx->pc++;
    }

    return TRANSLATE_OK;
}

cmd_read_err_t get_reg_val(const char* reg_name, int *reg_val)
{
    for (size_t i = 0; i < REGS_CNT; i++) {
        if (!strcmp(reg_name, registers[i].reg_name)) {
            ASM_LOG("[LOG]: Found register %s with code %lu\n", reg_name, i)
            *reg_val = registers[i].reg_val;
            return TRANSLATE_OK;
        }
    }

    return TRANSLATE_UNKNOWN_REG;
}