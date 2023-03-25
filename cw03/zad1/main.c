#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Invalid number of arguments");
        return 1;
    }

    int n = atoi(argv[1]);
    pid_t child_pid;

    for(int i=0; i<n; i++){
        child_pid = fork();
        if(child_pid == 0) {
            printf("%d %d\n", (int) getppid(), (int) getpid());
            return 0;
        }
    }

    while (wait(NULL) > 0);
    printf("%d\n", n);
    return 0;
}