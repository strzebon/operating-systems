#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define ELVES_TOTAL 10
#define ELVES_REQ 3
#define REINDEER_TOTAL 9

int elves = 0;
int reindeer = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition;


void *elf_function(void *arg){
    int *id = (int*) arg;
    while(1){
        sleep(2 + rand() % 4);
        pthread_mutex_lock(&mutex);

        if(elves < ELVES_REQ){
            elves++;
            if(elves == ELVES_REQ){
                printf("Elf: wybudzam Mikolaja, %d\n", *id);
                pthread_cond_broadcast(&condition);
            }
            else{
                printf("Elf: czeka %d elfow na Mikolaja, %d\n", elves, *id);
            }

            while(elves > 0){
                pthread_cond_wait(&condition, &mutex);
            }
        }
        else{
            printf("Elf: samodzielnie rozwiazuje swoj problem, %d\n", *id);
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *reindeer_function(void *arg){
    int *id = (int*) arg;
    while(1){
        sleep(5 + rand() % 6);
        pthread_mutex_lock(&mutex);
        reindeer++;

        if(reindeer == REINDEER_TOTAL){
            printf("Renifer: wybudzam Mikolaja, %d\n", *id);
            pthread_cond_broadcast(&condition);
        }
        else{
            printf("Renifer: czeka %d reniferow na Mikolaja, %d\n", reindeer, *id);
        }

        while(reindeer > 0){
            pthread_cond_wait(&condition, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(){
    int counter = 0;
    pthread_t elves_ids[ELVES_TOTAL];
    pthread_t reindeer_ids[REINDEER_TOTAL];

    for(int i=0; i<ELVES_TOTAL; i++){
        pthread_create(&elves_ids[i], NULL, elf_function, (void*) &elves_ids[i]);
    }
    for(int i=0; i<REINDEER_TOTAL; i++){
        pthread_create(&reindeer_ids[i], NULL, reindeer_function, (void*) &reindeer_ids[i]);
    }

    while(counter < 3){
        pthread_mutex_lock(&mutex);
        while(elves < ELVES_REQ && reindeer < REINDEER_TOTAL){
            pthread_cond_wait(&condition, &mutex);
        }

        if(elves == ELVES_REQ){
            printf("Mikolaj: rozwiazuje problemy elfow\n");
            sleep(1 + rand() % 2);
            elves = 0;
            pthread_cond_broadcast(&condition);
        }

        if(reindeer == REINDEER_TOTAL){
            printf("Mikolaj: dostarczam zabawki\n");
            sleep(2 + rand() % 3);
            counter++;
            reindeer = 0;
            pthread_cond_broadcast(&condition);
        }

        pthread_mutex_unlock(&mutex);
        printf("Mikolaj: zasypiam\n");
    }

    for(int i=0; i<ELVES_TOTAL; i++){
        pthread_cancel(elves_ids[i]);
    }
    for(int i=0; i<REINDEER_TOTAL; i++){
        pthread_cancel(reindeer_ids[i]);
    }

    return 0;
}