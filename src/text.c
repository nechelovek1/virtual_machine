#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>

#include "text.h"

int read_text(FILE* fp, textfile_t* textFile)
{
    if (fp == NULL || textFile == NULL) {
        return -1;
    }

    size_t fileSize = getFileSize(fp) + 1;

    textFile->buffer = (char* ) calloc(fileSize, sizeof(char));

    if(textFile->buffer == NULL) {
        return -1;
    }

    size_t bufferLen = fread(textFile->buffer, sizeof(char), fileSize, fp);
    textFile->bufferLen = bufferLen;
    textFile->buffer[textFile->bufferLen + 1] = '\0';
    

    size_t nLines = replaceChars(textFile->buffer, '\n', '\0') + 1;
    textFile->nLines = nLines;
    textFile->lines = (line_t* ) calloc(textFile->nLines, sizeof(line_t));

    if (textFile->lines == NULL) {
        return -1;
    }

    split_text(textFile);

    return 0;
}

void split_text(textfile_t* textFile)
{
    textFile->lines[0].line = textFile->buffer;

    size_t lineIdx = 1;
    for (size_t i = 1; i < textFile->bufferLen; i++)
    {
        if (textFile->buffer[i] == '\0' && lineIdx < textFile->nLines) {
            textFile->lines[lineIdx].line = textFile->buffer + (i + 1);
            textFile->lines[lineIdx - 1].len = textFile->lines[lineIdx].line - textFile->lines[lineIdx - 1].line - 1;
            lineIdx++;
        }
    }

    textFile->lines[textFile->nLines - 1].len = strlen(textFile->lines[textFile->nLines - 1].line);

    if (textFile->lines[textFile->nLines - 1].line[0] == '\0') {
        textFile->nLines--;
    }
}

size_t getFileSize(FILE* fp)
{
    int fd = fileno(fp);
    struct stat st = {};
    fstat(fd, &st);
    return st.st_size;
}

size_t getCharCount(const char* str, char c)
{
    size_t idx = 0;
    size_t cnt = 0;
    while (str[idx] != '\0')
    {
        if (str[idx] == c) {
            cnt++;
        }
        idx++;
    }
    return cnt;
}

int replaceChars(char* str, char oldC, char newC)
{
    size_t idx = 0;
    size_t cnt = 0;
    while (str[idx] != '\0')
    {
        if (str[idx] == oldC) {
            str[idx] = newC;
            cnt++;
        }
        idx++;
    }
    return cnt;
}

int fprintLines(FILE* fp, const textfile_t* textFile)
{
    if (fp == NULL || textFile == NULL) {
        return -1;
    }

    for (size_t i = 0; i < textFile->nLines; i++)
    {
        fprintf(fp, "%lu. %s\n", i, textFile->lines[i].line);
    }

    return 0;
}

int freeTextFile(textfile_t* textFile)
{
    if (textFile == NULL) {
        return -1;
    }

    free(textFile->buffer);
    free(textFile->lines);

    return 0;
}

void skipNonAlphaForward(const char* str, size_t* idx)
{
    while (str[*idx] != '\0' && !isalpha(str[*idx]))
    {
        (*idx)++;
    }
}

void skipNonAlphaBackward(const char* str, size_t* idx)
{
    while (idx > 0 && !isalpha(str[*idx]))
    {
        (*idx)--;
    }
}