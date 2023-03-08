#include "library.c"
#include <time.h>
#include <sys/times.h>
#define MAX_LENGTH 2000

int main(){
    struct timespec timespecStart, timespecEnd;
    struct tms tmsStart, tmsEnd;

    for(;;){
        clock_gettime(CLOCK_REALTIME, &timespecStart);
        times(&tmsStart);

        char *command = calloc(MAX_LENGTH, sizeof(char));
        fgets(command, MAX_LENGTH, stdin);
        char c;
        int i = 0;
        for(; i<MAX_LENGTH; i++){
            c = command[i];
            if(!isspace(c)) break;
        }
        char *first = calloc(MAX_LENGTH, sizeof(char));
        int j = 0;
        for(;i<MAX_LENGTH; i++){
            c = command[i];
            if(isspace(c)) break;
            first[j] = c;
            j++;
        }
        first = realloc(first,j * sizeof(char));

        for(; i<MAX_LENGTH; i++){
            c = command[i];
            if(!isspace(c)) break;
        }

        char *second = calloc(100, sizeof(char));
        int k = 0;
        for(;i<MAX_LENGTH; i++){
            c = command[i];
            if(isspace(c)) break;
            second[k] = c;
            k++;
        }
        second = realloc(second,k * sizeof(char));
        struct Array array;

        if(strcmp(first, "init") == 0){
            array = createArrayOfBlocks(atoi(second));
        }
        else if(strcmp(first, "count") == 0){
            wcProcedure(second, array);
        }
        else if(strcmp(first, "show") == 0){
            printf("%s \n", returnBlock(atoi(second), array));
        }
        else if(strcmp(first, "delete") == 0){
            removeBlock(atoi(second), array);
        }
        else if(strcmp(first, "destroy") == 0){
            removeArrayOfBlocks(array);
        }
        else{
            printf("Unknown command \n");
        }

        clock_gettime(CLOCK_REALTIME, &timespecEnd);
        times(&tmsEnd);

        printf("REAL: %ld ns\n", timespecEnd.tv_nsec - timespecStart.tv_nsec);
        printf("USER: %ld ticks\n", tmsEnd.tms_cutime - tmsStart.tms_cutime);
        printf("SYSTEM: %ld ticks\n", tmsEnd.tms_cstime - tmsStart.tms_cstime);
    }


    return 0;
}
