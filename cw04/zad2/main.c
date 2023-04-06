#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

int counter = 0;

void info_handler(int signo, siginfo_t* siginfo, void* context){
    printf("Signal number: %d\n",siginfo->si_signo);
    printf("PID: %d\n",siginfo->si_pid);
    printf("UID: %d\n",siginfo->si_uid);
    printf("Exit value or signal: %x\n",siginfo->si_status);
    printf("POSIX.1b signal: %d\n", siginfo->si_int);
}

void handler(int signo, siginfo_t* info, void* context){
    printf("Received signal\n");
    counter += 1;
    if(counter != 2){
        printf("Raised signal\n");
        raise(SIGUSR1);
    }
}

void test_SIGINFO(struct sigaction action){
    printf("SIGINFO\n");
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = info_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1,&action,NULL);

    printf("self\n");
    raise(SIGUSR1);
    printf("\n");

    printf("child\n");
    if(fork() == 0){
        raise(SIGUSR1);
        exit(0);
    }
    else wait(NULL);
    printf("\n");

    printf("custom\n");
    sigval_t val = {7};
    sigqueue(getpid(), SIGUSR1, val);
}

void test_NODEFER(struct sigaction action){
    printf("NODEFER\n");
    counter = 0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);
    printf("Raised signal\n");
    raise(SIGUSR1);
}

void test_RESETHAND(struct sigaction action){
    printf("RESETHAND\n");
    counter = 0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &action, NULL);
    printf("Raised signal\n");
    raise(SIGUSR1);
}

int main(int argc, char** argv){
    struct sigaction action;
    test_SIGINFO(action);
    printf("\n");
    test_NODEFER(action);
    printf("\n");
    test_RESETHAND(action);
    printf("\n");
    return 0;
}