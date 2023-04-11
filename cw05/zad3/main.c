#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
    if(argc != 3){
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    struct timespec timespecStart, timespecEnd;
    struct tms tmsStart, tmsEnd;
    clock_gettime(CLOCK_REALTIME, &timespecStart);
    times(&tmsStart);

    double h = atof(argv[1]);
    int n = atoi(argv[2]);
    char path[] = "/tmp/fifo";
    int fd;
    double s = 0;
    char buf[2048];
    char a[100];
    char b[100];
    int counter = 0;

    mkfifo(path, 0666);
    for(int i=0; i<n; i++){
        sprintf(a, "%lf", (double) i/n);
        sprintf(b, "%lf", (double) (i+1)/n);

        if(fork() == 0){
            execl("./integral", "integral", argv[1], a, b, NULL);
        }
    }

    fd = open(path, O_RDONLY);

    while(counter < n){
        size_t size = read(fd, buf, 2048);
        buf[size] = 0;
        char* token = strtok(buf, ",");
        while(token != NULL){
            s += atof(buf);
            token  = strtok(NULL, ",");
            counter++;
        }
    }

    close(fd);
    remove(path);

    printf("h = %.9f\n", h);
    printf("n = %d\n", n);
    printf("RESULT: %lf\n", s);

    clock_gettime(CLOCK_REALTIME, &timespecEnd);
    times(&tmsEnd);
    printf("TIME: %lf s\n\n", (double)(timespecEnd.tv_sec - timespecStart.tv_sec) + (double)(timespecEnd.tv_nsec - timespecStart.tv_nsec)/1e9);

    return 0;
}