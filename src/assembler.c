#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//#include "text.h"
#include "text.h"
#include "stack.h"
#include "commands.h"

typedef enum {
    TRANSLATE_OK,
    TRANSLATE_TOO_MANY_ARGS,
    TRANSLATE_MISSING_ARGS,
    TRANSLATE_UNKNOWN_CMD,
    TRANSLATE_UNKNOWN_REG
} cmd_read_err_t;

typedef struct {
    char cmd_word[5];
    int cmd_code;
    size_t arg_cnt;
    bool isargreg;
    bool isarglabel;
} cmd_struct_t;

typedef struct
{
    char reg_name[3];
    int reg_val;
} reg_t;

typedef struct {
    int value;
    int address;
} label_t;

typedef struct {
    textfile_t text;
    label_t* labels;
    size_t labels_cnt;
    int* bytecode;
    size_t bytecode_size;
    bool mode;
} translate_ctx_t;

const cmd_struct_t commands[] = {
    {"PUSH", 1,  1, 0, 0},
    {"ADD",  2,  0, 0, 0},
    {"SUB",  3,  0, 0, 0},
    {"DIV",  4,  0, 0, 0},
    {"MUL",  5,  0, 0, 0},
    {"OUT",  6,  0, 0, 0},
    {"HLT",  7,  0, 0, 0},
    {"PUSHR", 8,  1, 1, 0},
    {"POPR",  9,  1, 1, 0},
    {"JMP",  10, 1, 0, 1},
    {"JA",   11, 1, 0, 1},
    {"JAE",  12, 1, 0, 1},
    {"JB",   13, 1, 0, 1},
    {"JBE",  14, 1, 0, 1},
    {"JE",   15, 1, 0, 1},
    {"JNE",  16, 1, 0, 1},
    {"IN",   17, 0, 0, 0},
    {"CALL", 18, 1, 0, 1},
    {"RET",  19, 1, 0, 1},
    {"PUSHM", 20, 1, 0, 1},
    {"POPM", 21, 1, 0, 1}
};

cmd_read_err_t translate_text(translate_ctx_t* ctx);
cmd_read_err_t read_cmd(translate_ctx_t* ctx, line_t cmd_line);
int find_label(translate_ctx_t* ctx, int label_value);
int read_label(translate_ctx_t* ctx, line_t label_line, int label_address);
int create_label(translate_ctx_t* ctx, int label_value, int label_address);
int read_jump(translate_ctx_t* ctx, int cmd_code, line_t cmd_line, size_t* cmd_len);
int read_register(int* reg, line_t cmd_line, size_t* cmd_len);
int translate_context_read_text(translate_ctx_t* ctx, FILE* fp);
void print_labels(translate_ctx_t* ctx);

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <programm file> <bytecode file>\n", argv[0]);
        return -1;
    }

    textfile_t text = {};
    FILE* input_file = NULL;
    if ((input_file = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open %s", argv[1]);
        return -1;
    }

    translate_ctx_t ctx = {};
    translate_context_read_text(&ctx, input_file);

    fclose(input_file);

    FILE* out_file = NULL;
    if ((out_file = fopen(argv[2], "wb")) == NULL) {
        printf("Failed to open %s", argv[2]);
        return -1;
    }

    ctx.mode = true; //Первый проход
    translate_text(&ctx);
    ctx.bytecode_size = 0;
    ctx.mode = false; //Второй проход
    translate_text(&ctx);

    fwrite(ctx.bytecode, ctx.bytecode_size, sizeof(label_t), out_file);

    fclose(out_file);

    return 0;
}

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
    char cmd_word[6] = "";
    sscanf(cmd_line.line, "%5s", cmd_word);
    size_t cmd_len = strlen(cmd_word);

    if (!strcmp(cmd_word, "PUSH")) {
        ctx->bytecode[ctx->bytecode_size] = PUSH;
        ctx->bytecode_size++;

        int elem = 0, n = 0;
        int res = sscanf(cmd_line.line + cmd_len, "%d%n", &elem, &n);
        cmd_len += n;
        if (res != 1) {
            return TRANSLATE_MISSING_ARGS;
        }

        ctx->bytecode[ctx->bytecode_size] = elem;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "ADD")) {
        ctx->bytecode[ctx->bytecode_size] = ADD;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "SUB")) {
        ctx->bytecode[ctx->bytecode_size] = SUB;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "DIV")) {
        ctx->bytecode[ctx->bytecode_size] = DIV;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "MUL")) {
        ctx->bytecode[ctx->bytecode_size] = MUL;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "OUT")) {
        ctx->bytecode[ctx->bytecode_size] = OUT;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "HLT")) {
        ctx->bytecode[ctx->bytecode_size] = HLT;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "PUSHR")) {
        ctx->bytecode[ctx->bytecode_size] = PUSHR;
        ctx->bytecode_size++;
        if (read_register(&ctx->bytecode[ctx->bytecode_size], cmd_line, &cmd_len)) {
            return TRANSLATE_UNKNOWN_REG;
        }
        ctx->bytecode_size++;
        
    } else if (!strcmp(cmd_word, "POPR")) {
        ctx->bytecode[ctx->bytecode_size] = POPR;
        ctx->bytecode_size++;
        if (read_register(&ctx->bytecode[ctx->bytecode_size], cmd_line, &cmd_len)) {
            return TRANSLATE_UNKNOWN_REG;
        }
        ctx->bytecode_size++;

    } else if (!strcmp(cmd_word, "JMP")) {
        if (read_jump(ctx, JMP, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JA")) {
        if (read_jump(ctx, JA, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JAE")) {
        if (read_jump(ctx, JAE, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JB")) {
        if (read_jump(ctx, JB, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JBE")) {
        if (read_jump(ctx, JBE, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JE")) {
        if (read_jump(ctx, JE, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "JNE")) {
        if (read_jump(ctx, JNE, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }    
    } else if (!strcmp(cmd_word, "IN")) {
        ctx->bytecode[ctx->bytecode_size] = IN;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "CALL")) {
        if (read_jump(ctx, CALL, cmd_line, &cmd_len)) {
            return TRANSLATE_MISSING_ARGS;
        }
    } else if (!strcmp(cmd_word, "RET")) {
        ctx->bytecode[ctx->bytecode_size] = RET;
        ctx->bytecode_size++;
    } else if (!strcmp(cmd_word, "PUSHM")) {
        ctx->bytecode[ctx->bytecode_size] = PUSHM;
        ctx->bytecode_size++;
        ctx->bytecode_size++;

    } else if (!strcmp(cmd_word, "POPM")) {
        ctx->bytecode[ctx->bytecode_size] = POPM;
        ctx->bytecode_size++;
        ctx->bytecode_size++;

    } else {
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

int read_label(translate_ctx_t* ctx, line_t label_line, int label_address)
{
    if (!ctx->mode) {
        return 0;
    }

    int label_value = 0, n = 0;
    int res = sscanf(label_line.line, "%d%n", &label_value, &n);
    
    if (res != 1) {
        return -1;
    }

    if (label_line.line[n] != ':') {
        return -2;
    }

    int find_res = find_label(ctx, label_value);

    if (find_res == -1) {
        create_label(ctx, label_value, label_address);
        return 0;
    } 

    if (ctx->labels[find_res].address == -1) {
        ctx->labels[find_res].address = label_address;
        return 0;
    }

    return -3;
}

int read_jump(translate_ctx_t* ctx, int cmd_code, line_t cmd_line, size_t* cmd_len)
{
    ctx->bytecode[ctx->bytecode_size] = cmd_code;
    ctx->bytecode_size++;

    int label_value = 0, n = 0;
    int res = sscanf(cmd_line.line + *cmd_len, "%d%n", &label_value, &n);
    
    if (res != 1) {
        return -1;
    }
    
    *cmd_len += n;

    if (cmd_line.line[*cmd_len] == ':') {
        ctx->bytecode[ctx->bytecode_size] = find_label(ctx, label_value);
        if (ctx->bytecode[ctx->bytecode_size] == -1) {
            if (ctx->mode) {
                create_label(ctx, label_value, -1);
            } else {
                return -1;
            }
        }
    } else {
        ctx->bytecode[ctx->bytecode_size] = label_value;
    }
    ctx->bytecode_size++;

    return 0;
}

int read_register(int* reg, line_t cmd_line, size_t* cmd_len)
{
    char reg_name[3] = "";
    int n = 0;

    sscanf(cmd_line.line + *cmd_len, "%2s%n", reg_name, &n);
    *cmd_len += n;

    if (!strcmp(reg_name, "AX")) {
        *reg = 0;
    } else if (!strcmp(reg_name, "BX")) {
        *reg = 1;
    } else if (!strcmp(reg_name, "CX")) {
        *reg = 2;
    } else if (!strcmp(reg_name, "DX")) {
        *reg = 3;
    } else {
        return -1;
    }
    
    return 0;
}

void print_labels(translate_ctx_t* ctx)
{
    for (size_t i = 0; i < ctx->labels_cnt; i++) {
        printf("lbl: %d addr: %d\n", ctx->labels[i].value, ctx->labels[i].address);
    }
}