#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define ELVES_TOTAL 10
#define ELVES_REQ 3
#define REINDEER_TOTAL 9

int elves = 0;
int reindeer = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_condition = PTHREAD_COND_INITIALIZER;
pthread_t elves_waiting_ids[ELVES_REQ];

void handler(){
    printf("Elf: Mikolaj rozwiazuje problem, %lu\n", pthread_self());
}

void *elf_function(void *arg){
    signal(SIGUSR1, handler);
    pthread_t id = pthread_self();

    while(1){
        sleep(2 + rand() % 4);
        pthread_mutex_lock(&mutex);

        if(elves < ELVES_REQ){
            elves_waiting_ids[elves] = id;
            elves++;
            printf("Elf: czeka %d elfow na Mikolaja, %lu\n", elves, id);
            if(elves == ELVES_REQ){
                printf("Elf: wybudzam Mikolaja, %lu\n", id);
                pthread_cond_signal(&santa_condition);
            }

            while(elves > 0){
                pthread_cond_wait(&elf_condition, &mutex);
            }
        }
        else{
            printf("Elf: samodzielnie rozwiazuje swoj problem, %lu\n", id);
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *reindeer_function(void *arg){
    pthread_t id = pthread_self();
    while(1){
        sleep(5 + rand() % 6);
        pthread_mutex_lock(&mutex);
        reindeer++;
        printf("Renifer: czeka %d reniferow na Mikolaja, %lu\n", reindeer, id);

        if(reindeer == REINDEER_TOTAL){
            printf("Renifer: wybudzam Mikolaja, %lu\n", id);
            pthread_cond_signal(&santa_condition);
        }

        while(reindeer > 0){
            pthread_cond_wait(&reindeer_condition, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void solve_elves_problems(){
    printf("Mikolaj: rozwiazuje problemy elfow ");
    for(int i=0; i<ELVES_REQ; i++){
        printf("%lu ", elves_waiting_ids[i]);
    }
    printf("\n");
    for(int i=0; i<ELVES_REQ; i++){
        pthread_kill(elves_waiting_ids[i], SIGUSR1);
    }
    pthread_mutex_unlock(&mutex);
    sleep(1 + rand() % 2);
    pthread_mutex_lock(&mutex);
    elves = 0;
    pthread_cond_broadcast(&elf_condition);
}

void *santa_function(void *arg){
    int counter = 0;
    while(1){
        pthread_mutex_lock(&mutex);
        while(elves < ELVES_REQ && reindeer < REINDEER_TOTAL){
            pthread_cond_wait(&santa_condition, &mutex);
        }
        printf("Mikolaj: budze sie\n");

        if(elves == ELVES_REQ){
            solve_elves_problems();
        }

        if(reindeer == REINDEER_TOTAL){
            printf("Mikolaj: dostarczam zabawki\n");
            pthread_mutex_unlock(&mutex);
            sleep(2 + rand() % 3);
            pthread_mutex_lock(&mutex);
            counter++;
            if(counter == 3){
                printf("Mikolaj: koniec pracy\n");
                raise(SIGINT);
            }
            reindeer = 0;
            pthread_cond_broadcast(&reindeer_condition);
        }

        if(elves == ELVES_REQ){
            solve_elves_problems();
        }

        printf("Mikolaj: zasypiam\n");
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(){
    pthread_t elves_ids[ELVES_TOTAL];
    pthread_t reindeer_ids[REINDEER_TOTAL];
    pthread_t santa_id;

    pthread_create(&santa_id, NULL, santa_function, NULL);

    for(int i=0; i<ELVES_TOTAL; i++){
        pthread_create(&elves_ids[i], NULL, elf_function, NULL);
    }

    for(int i=0; i<REINDEER_TOTAL; i++){
        pthread_create(&reindeer_ids[i], NULL, reindeer_function, NULL);
    }

    pause();

    return 0;
}