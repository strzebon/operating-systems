#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>

void sys_replace_char(char** argv){
    char c;
    int we,wy;
    we=open(argv[3], O_RDONLY);
    wy=open(argv[4], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    while(read(we,&c,1) == 1) {
        if (c == argv[1][0]) c = argv[2][0];
        write(wy, &c, 1);
    }
    close(we);
    close(wy);
}

void lib_replace_char(char** argv){
    char c;
    FILE *we, *wy;
    we = fopen(argv[3], "r");
    wy = fopen(argv[4], "w");
    while(fread(&c, sizeof(char), 1, we) == 1){
        if (c == argv[1][0]) c = argv[2][0];
        fwrite(&c, sizeof(char), 1, wy);
    }
    fclose(we);
    fclose(wy);
}


int main(int argc, char** argv){
    if (argc != 5) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    struct timespec timespecStart, timespecEnd;
    struct tms tmsStart, tmsEnd;
    clock_gettime(CLOCK_REALTIME, &timespecStart);
    times(&tmsStart);

#ifdef SYS
    sys_replace_char(argv);
#else
    lib_replace_char(argv);
#endif

    clock_gettime(CLOCK_REALTIME, &timespecEnd);
    times(&tmsEnd);

    printf("\nREAL: %lf s\n", (double)(timespecEnd.tv_sec - timespecStart.tv_sec) + (double)(timespecEnd.tv_nsec - timespecStart.tv_nsec)/1e9);
    printf("USER: %ld ticks\n", tmsEnd.tms_cutime - tmsStart.tms_cutime);
    printf("SYSTEM: %ld ticks\n", tmsEnd.tms_cstime - tmsStart.tms_cstime);

    return 0;
}