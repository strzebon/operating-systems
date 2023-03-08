#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct Array {
    int *size;
    int *numberOfSaved;
    char **arrayOfBlocks;
};

struct Array createArrayOfBlocks(int size);

void wcProcedure(char *fileName, struct Array array);

char* returnBlock(int index, struct Array array);

void removeBlock(int index, struct Array array);

void removeArrayOfBlocks(struct Array array);



