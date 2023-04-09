#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>

double integral(double h, double a, double b){
    double s = 0;
    double x;
    for(x=a; x+h<b; x+=h){
        s += h * 4/((x + 0.5*h)*(x + 0.5*h) + 1);
    }
    h = b-x;
    s += h * 4/((x + 0.5*h)*(x + 0.5*h) + 1);
    return s;
}

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
    int fd[n][2];
    char buf[100];

    for(int i=0; i<n; i++){
        pipe(fd[i]);

        if(fork() == 0){
            close(fd[i][0]);
            sprintf(buf, "%lf", integral(h, (double) i/n, (double) (i+1)/n));
            write(fd[i][1], buf, 100);
            return 0;
        }
        else close(fd[i][1]);
    }
    while (wait(NULL) > 0);

    double s = 0;
    for(int i=0; i<n; i++){
        read(fd[i][0], buf, 100);
        s += atof(buf);
    }

    printf("h = %.9f\n", h);
    printf("n = %d\n", n);
    printf("RESULT: %lf\n", s);

    clock_gettime(CLOCK_REALTIME, &timespecEnd);
    times(&tmsEnd);
    printf("TIME: %lf s\n\n", (double)(timespecEnd.tv_sec - timespecStart.tv_sec) + (double)(timespecEnd.tv_nsec - timespecStart.tv_nsec)/1e9);

    return 0;
}