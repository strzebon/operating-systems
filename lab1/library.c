#include "library.h"
#define MAX_SIZE 200
#define TEMP_PATH "./tmp/tmp.txt"


struct Array createArrayOfBlocks(int size){
    struct Array array;
    array.size = malloc(sizeof(int));
    array.numberOfSaved = malloc(sizeof(int));
    array.arrayOfBlocks = (char **) calloc(size, sizeof(char *));
    *array.size = size;
    *array.numberOfSaved = 0;
    return array;
}

void wcProcedure(char *fileName, struct Array array){
    char *command = calloc(MAX_SIZE, sizeof(char));
    for(int i = 0; i < MAX_SIZE; i++){
        command[i] = 0;
    }

    strcpy(command, "wc ");
    strcat(command, fileName);
    strcat(command, " > ");
    strcat(command, TEMP_PATH);
    system(command);

    FILE *file = fopen(TEMP_PATH, "r");
    if(file == NULL) {
        perror("fopen failed \n");
    }
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
//    rewind(file);
    array.arrayOfBlocks[*array.numberOfSaved] = calloc(fileSize + 1, sizeof(char));
    fread(array.arrayOfBlocks[*array.numberOfSaved], sizeof(char), fileSize, file);
    fclose(file);
    (*array.numberOfSaved)++;
}

char* returnBlock(int index, struct Array array){
    return array.arrayOfBlocks[index];
}

void removeBlock(int index, struct Array array){
    if(array.arrayOfBlocks[index] == NULL) return;
    memmove(&array.arrayOfBlocks[index], &array.arrayOfBlocks[index+1], (*array.size - index - 1) * sizeof(char *));
    (*array.numberOfSaved)--;
}

void removeArrayOfBlocks(struct Array array){
    free(array.arrayOfBlocks);
    free(array.numberOfSaved);
    free(array.numberOfSaved);
}
