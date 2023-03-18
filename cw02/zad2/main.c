#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#ifndef BLOCK_SIZE
    #define BLOCK_SIZE 1
#endif

void reverse(int size, char* block){
    char tmp;
    for(int i=0; i<size/2; i++){
        tmp = block[size-i-1];
        block[size-i-1] = block[i];
        block[i] = tmp;
    }
}

int main(int argc, char** argv){
    if (argc != 3) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    struct timespec timespecStart, timespecEnd;
    struct tms tmsStart, tmsEnd;
    clock_gettime(CLOCK_REALTIME, &timespecStart);
    times(&tmsStart);

    FILE *we, *wy;
    we = fopen(argv[1], "r");
    wy = fopen(argv[2], "w");
    fseek(we, 0, SEEK_END);
    int file_size = (int) ftell(we);
    fseek(we, 0, SEEK_SET);
    int number_of_blocks = file_size / BLOCK_SIZE;
    int remainder = file_size % BLOCK_SIZE;
    char block[BLOCK_SIZE];

    for(int i=0; i<number_of_blocks; i++){
        fseek(we, -(i+1) * BLOCK_SIZE, SEEK_END);
        fread(block, sizeof(char), BLOCK_SIZE, we);
        reverse(BLOCK_SIZE, block);
        fwrite(block, sizeof(char), BLOCK_SIZE, wy);
    }

    fseek(we, 0, SEEK_SET);
    fread(block, sizeof(char), remainder, we);
    reverse(remainder, block);
    fwrite(block, sizeof(char), remainder, wy);

    fclose(we);
    fclose(wy);

    clock_gettime(CLOCK_REALTIME, &timespecEnd);
    times(&tmsEnd);
    printf("\nREAL: %lf s\n", (double)(timespecEnd.tv_sec - timespecStart.tv_sec) + (double)(timespecEnd.tv_nsec - timespecStart.tv_nsec)/1e9);
    printf("USER: %ld ticks\n", tmsEnd.tms_cutime - tmsStart.tms_cutime);
    printf("SYSTEM: %ld ticks\n", tmsEnd.tms_cstime - tmsStart.tms_cstime);

    return 0;
}