#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int counter = 0;
int task = 0;
int done = 1;

void print_time(){
    time_t raw_time;
    struct tm * time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    printf ("[catcher] current time: %s", asctime(time_info));
}

void task1(){
    done = 1;
    for(int i=1; i<=100; i++){
        printf("[catcher] %d\n", i);
    }
}

void task2(){
    done = 1;
    print_time();
}

void task3(){
    done = 1;
    printf("[catcher] number of valid calls: %d\n", counter);
}

void task4(){
    while(task == 4){
        print_time();
        sleep(1);
    }
}

void task5(){
    printf("[catcher] finished\n");
    exit(0);
}

void handler(int signo, siginfo_t* info, void* context){
    task = info->si_status;
    if(task < 1 || task > 5) printf("[catcher] invalid call\n");
    else counter++;
    kill(info->si_pid, SIGUSR1);
    done = 0;
}

int main(){
    printf("[catcher] pid: %d\n", getpid());

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    while(1){
        if(done) continue;
        switch(task){
            case 1: task1(); break;
            case 2: task2(); break;
            case 3: task3(); break;
            case 4: task4(); break;
            case 5: task5(); break;
            default: break;
        }
    }
}