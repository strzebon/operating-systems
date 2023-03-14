#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct Array {
    int *size;
    int *numberOfSaved;
    char **arrayOfBlocks;
};

struct Array init(int size);

void count(char *fileName, struct Array array);

char* block(int index, struct Array array);

void delete(int index, struct Array array);

void destroy(struct Array array);



