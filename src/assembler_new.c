#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//#include "text.h"
#include "text.h"

typedef enum {
    TRANSLATE_OK,
    TRANSLATE_TOO_MANY_ARGS,
    TRANSLATE_MISSING_ARGS,
    TRANSLATE_UNKNOWN_CMD
} cmd_read_err_t;

typedef struct {
    char cmd_word[5];
    int cmd_code;
    size_t arg_cnt;
    bool isargreg;
    bool isarglabel;
} cmd_struct_t;

typedef struct {
    int value;
    int address;
} label_t;

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

cmd_read_err_t translate_cmd(FILE* out_file, line_t cmd_line, const cmd_struct_t* cmd_list, size_t cmd_cnt);

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
    read_text(input_file, &text);
    
    size_t bytecode_size = getFileSize(input_file);
    //int bytecode = calloc(bytecode_size, sizeof(int));

    fclose(input_file);

    FILE* out_file = NULL;
    if ((out_file = fopen(argv[2], "w")) == NULL) {
        printf("Failed to open %s", argv[2]);
        return -1;
    }

    for (size_t i = 0; i < text.nLines; i++)
    {
        line_t cmd_line = text.lines[i];
        if (cmd_line.len == 0) {
            continue;
        }

        if (translate_cmd(out_file, cmd_line, commands, sizeof(commands)) != TRANSLATE_OK) {
            printf("Error on line %lu\n", i + 1);
            fclose(out_file);
            return -1;
        }
    }

    fclose(out_file);

    return 0;
}

cmd_read_err_t translate_cmd(FILE* out_file, line_t cmd_line, const cmd_struct_t* cmd_list, size_t cmd_cnt)
{
    const size_t MAX_CMD_LEN = 4; 
    char word[5] = "";
    sscanf(cmd_line.line, "%4s", word);
    size_t cmd_word_len = strlen(word);

    for (size_t cmd = 0; cmd < cmd_cnt; cmd++) {
        if (!strcmp(word, cmd_list[cmd].cmd_word)) {
            fprintf(out_file, "%d", cmd_list[cmd].cmd_code);

            size_t cmd_full_len = cmd_word_len;
            for (size_t arg = 0; arg < cmd_list[cmd].arg_cnt; arg++) {
                int arg_value = 0;
                int arg_len = 0;

                if(sscanf(cmd_line.line + cmd_full_len, "%d %n", &arg_value, &arg_len) == 1) {
                    fprintf(out_file, " %d", arg_value);
                    cmd_full_len += arg_len;
                } else {
                    return TRANSLATE_MISSING_ARGS;
                }
            }

            if (cmd_full_len != cmd_line.len) {
                return TRANSLATE_TOO_MANY_ARGS;
            }

            fprintf(out_file, "\n");
            return TRANSLATE_OK;
        }
    }

    return TRANSLATE_UNKNOWN_CMD;
}