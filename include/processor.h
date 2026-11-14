#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdlib.h>

#include "commands.h"

//extern const size_t MAX_CMD_NUM;
const size_t STACK_MIN_CAPACITY = 5;

int processor_read_bytecode(processor_t* proc, FILE* fp);
int processor_init(processor_t* proc, int height, int width);
int processor_loop(processor_t* proc);
int processor_destroy(processor_t* proc);
void processor_dump(processor_t* proc);

#endif