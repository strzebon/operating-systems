#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#define SIGNAL SIGUSR1

void block_signal(){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGNAL);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void handler(int sig_no){
    printf("[%d] Received signal\n", getpid());
}

void raise_signal(){
    printf("[%d] Raised signal\n", getpid());
    raise(SIGNAL);
}

void is_signal_pending(){
    sigset_t sigset;
    sigpending(&sigset);
    if(sigismember(&sigset, SIGNAL)) printf("[%d] Signal is pending\n", getpid());
    else printf("[%d] Signal is not pending\n", getpid());
}

void parent_function(char* task){
    if(strcmp(task, "ignore") == 0){
        signal(SIGNAL, SIG_IGN);
        raise_signal();
    }
    else if(strcmp(task, "handler") == 0){
        signal(SIGNAL, handler);
        raise_signal();
    }
    else if(strcmp(task, "mask") == 0){
        signal(SIGNAL, handler);
        block_signal();
        raise_signal();
    }
    else if(strcmp(task, "pending") == 0){
        signal(SIGNAL, handler);
        block_signal();
        raise_signal();
        is_signal_pending();
    }
    else fprintf(stderr, "Invalid argv[1]");
}

int main(int argc, char** argv){

    if(argc != 3){
        fprintf(stderr, "Invalid number of arguments");
    }

    if(strcmp(argv[2], "0") == 0){
        parent_function(argv[1]);
        if(fork() == 0){
            if(strcmp(argv[1], "pending") == 0) is_signal_pending();
            else raise_signal();
        }
        else wait(NULL);
    }

    else if(strcmp(argv[2], "1") == 0){
        parent_function(argv[1]);
        execl(argv[0], argv[0], argv[1], "2", NULL);
    }

    else if(strcmp(argv[2], "2") == 0){
        if(strcmp(argv[1], "pending") == 0) is_signal_pending();
        else raise_signal();
    }

    else{
        fprintf(stderr, "Invalid argv[2]");
        return 1;
    }

    return 0;
}