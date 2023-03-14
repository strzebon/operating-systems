#include "library.h"


struct Array init(int size){
    struct Array array;
    array.size = malloc(sizeof(int));
    array.numberOfSaved = malloc(sizeof(int));
    array.arrayOfBlocks = (char **) calloc(size, sizeof(char *));
    *array.size = size;
    *array.numberOfSaved = 0;
    return array;
}

void count(char *fileName, struct Array array){
    char *command = calloc(MAX_SIZE, sizeof(char));
//    for(int i = 0; i < MAX_SIZE; i++){
//        command[i] = 0;
//    }
    char tmp[] = "/tmp/tmp_XXXXXX";
    mkstemp(tmp);

    strcpy(command, "wc ");
    strcat(command, fileName);
    strcat(command, " > ");
    strcat(command, tmp);
    system(command);

    FILE *file = fopen(tmp, "r");
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    array.arrayOfBlocks[*array.numberOfSaved] = calloc(fileSize + 1, sizeof(char));
    fread(array.arrayOfBlocks[*array.numberOfSaved], sizeof(char), fileSize, file);
    fclose(file);
    (*array.numberOfSaved)++;
    strcpy(command, "rm -f ");
    strcat(command, tmp);
    system(command);
    free(command);
}

char* block(int index, struct Array array){
    return array.arrayOfBlocks[index];
}

void delete(int index, struct Array array){
    if(array.arrayOfBlocks[index] == NULL) return;
    memmove(&array.arrayOfBlocks[index], &array.arrayOfBlocks[index+1], (*array.size - index - 1) * sizeof(char *));
    (*array.numberOfSaved)--;
}

void destroy(struct Array array){
    free(array.arrayOfBlocks);
    free(array.numberOfSaved);
    free(array.size);
}
