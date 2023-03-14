#include "library.h"
#include "dllmanager.h"
#include <time.h>
#include <sys/times.h>

int main(){
    load_dll("library.so");
    struct timespec timespecStart, timespecEnd;
    struct tms tmsStart, tmsEnd;
    char *first;
    char *second;

    for(;;){
        char *command = calloc(MAX_SIZE, sizeof(char));
        first = calloc(MAX_SIZE, sizeof(char));
        second = calloc(MAX_SIZE, sizeof(char));

        printf(">>");
        fgets(command, MAX_SIZE, stdin);
        sscanf(command, "%s%s", first, second);
//        printf("%s\n", first);
//        printf("%s \n", second);

        struct Array array;

        clock_gettime(CLOCK_REALTIME, &timespecStart);
        times(&tmsStart);

        if(strcmp(first, "init") == 0){
            array = init(atoi(second));
        }
        else if(strcmp(first, "count") == 0){
            count(second, array);
        }
        else if(strcmp(first, "show") == 0){
            printf("\n%s", block(atoi(second), array));
        }
        else if(strcmp(first, "delete") == 0){
            delete(atoi(second), array);
        }
        else if(strcmp(first, "destroy") == 0){
            destroy(array);
        }
        else if(strcmp(first, "exit") == 0){
            free(command);
            free(first);
            free(second);
            break;
        }
        else
        {
            printf("unknown command \n");
        }

        clock_gettime(CLOCK_REALTIME, &timespecEnd);
        times(&tmsEnd);

        printf("\nREAL: %ld ns\n", timespecEnd.tv_nsec - timespecStart.tv_nsec);
        printf("USER: %ld ticks\n", tmsEnd.tms_cutime - tmsStart.tms_cutime);
        printf("SYSTEM: %ld ticks\n", tmsEnd.tms_cstime - tmsStart.tms_cstime);

        free(first);
        free(second);
    }
    close_dll();
    return 0;
}
