#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 2000

struct Array {
    int *size;
    int *numberOfSaved;
    char **arrayOfBlocks;
};

#ifdef USE_DLL

struct Array (*init)(int size);

void (*count)(char *fileName, struct Array array);

char* (*block)(int index, struct Array array);

void (*delete)(int index, struct Array array);

void (*destroy)(struct Array array);

#else

struct Array init(int size);

void count(char *fileName, struct Array array);

char* block(int index, struct Array array);

void delete(int index, struct Array array);

void destroy(struct Array array);

#endif
#endif



