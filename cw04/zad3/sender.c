#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handler(int signo){
    printf("[sender] got confirmation from catcher\n");
}

int main(int argc, char** argv){
    if(argc < 3){
        fprintf(stderr, "[sender] invalid number of arguments\n");
        return 1;
    }

    int catcher_pid = atoi(argv[1]);
    int task;
    signal(SIGUSR1, handler);
    for(int i=2; i<argc; i++){
        task = atoi(argv[i]);
        sigval_t val = {task};
        printf("[sender] raised signal\n");
        sigqueue(catcher_pid, SIGUSR1, val);
        pause();
    }

    return 0;
}