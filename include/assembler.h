#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdlib.h>

#include "text.h"
#include "commands.h"

typedef enum {
    TRANSLATE_OK,
    TRANSLATE_TOO_MANY_ARGS,
    TRANSLATE_MISSING_ARGS,
    TRANSLATE_UNKNOWN_CMD,
    TRANSLATE_UNKNOWN_REG,
    TRANSLATE_BAD_LABEL,
    TRANSLATE_LABEL_EXIST
} cmd_read_err_t;


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


cmd_read_err_t translate_text(translate_ctx_t* ctx);
cmd_read_err_t read_cmd(translate_ctx_t* ctx, line_t cmd_line);
int find_label(translate_ctx_t* ctx, int label_value);
cmd_read_err_t read_label(translate_ctx_t* ctx, line_t label_line, int label_address);
int create_label(translate_ctx_t* ctx, int label_value, int label_address);
cmd_read_err_t read_jump(translate_ctx_t* ctx, command_t cmd_code, line_t cmd_line, size_t cmd_len);
cmd_read_err_t read_register(int* reg, line_t cmd_line, size_t* cmd_len);
cmd_read_err_t read_command_word(const char* line, size_t* cmd_len, command_t* cmd_code);
int translate_context_read_text(translate_ctx_t* ctx, FILE* fp);
int translate_context_destroy(translate_ctx_t* ctx);
void print_labels(translate_ctx_t* ctx);
void assembler_dump(translate_ctx_t* ctx);

#endif