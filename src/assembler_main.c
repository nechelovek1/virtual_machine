#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//#include "text.h"
#include "text.h"
#include "stack.h"
#include "commands.h"
#include "assembler.h"


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

    ctx.mode = true; //Первый проход
    if (translate_text(&ctx) != TRANSLATE_OK) {
        return -1;
    }
    print_labels(&ctx);

    ctx.bytecode_size = 0;
    ctx.mode = false; //Второй проход
    if (translate_text(&ctx) != TRANSLATE_OK) {
        return -1;
    }
    print_labels(&ctx);

    FILE* out_file = NULL;
    if ((out_file = fopen(argv[2], "wb")) == NULL) {
        printf("Failed to open %s", argv[2]);
        return -1;
    }

    fwrite(ctx.bytecode, ctx.bytecode_size, sizeof(int), out_file);

    translate_context_destroy(&ctx);
    fclose(out_file);

    return 0;
}

