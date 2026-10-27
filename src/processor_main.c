#include <stdio.h>
#include <stdbool.h>

#include "processor.h"
#include "stack.h"
#include "text.h"
#include "commands.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <byte code file>\n", argv[0]);
        return -1;
    }

    FILE* file = NULL;
    if ((file = fopen(argv[1], "r")) == NULL) {
        printf("Failed to open %s\n", argv[1]);
        return -1;
    }

    processor_t proc = {};

    processor_init(&proc);
    processor_read_bytecode(&proc, file);

    fclose(file);

    processor_loop(&proc);
    processor_destroy(&proc);
    return 0;
}

