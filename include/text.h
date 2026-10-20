#ifndef TEXT_H
#define TEXT_H

#include <stdio.h>

typedef struct {
    char* line;
    size_t len;
} line_t;

typedef struct {
    char* buffer;
    size_t bufferLen;
    line_t* lines;
    size_t nLines;
} textfile_t;

int read_text(FILE* fp, textfile_t* textFile);
void split_text(textfile_t* textFile);
size_t getFileSize(FILE* fp);
size_t getCharCount(const char* str, char c);
int replaceChars(char* str, char oldC, char newC);
int fprintLines(FILE* fp, const textfile_t* textFile);
int freeTextFile(textfile_t* textFile);
void skipNonAlphaForward(const char* str, size_t* idx);
void skipNonAlphaBackward(const char* str, size_t* idx);

#endif