#include <stdlib.h>

#include "semaphore.h"

int create_semaphore(const char *filename, int initial) {
    key_t key = ftok(getenv("HOME"), filename[0]);
    if (key == -1) {
        perror("Creating a semaphore failed on ftok");
        return -1;
    }
    int semid = semget(key, 1, 0664 | IPC_CREAT);
    if (semid == -1) {
        perror("Creating a semaphore failed on semid");
        return -1;
    }
    if(semctl(semid, 0, SETVAL, initial) == -1) {
        perror("Creating a semaphore failed on semctl");
        return -1;
    }
    return semid;
}

int open_semaphore(const char *filename) {
    key_t key = ftok(getenv("HOME"), filename[0]);
    if (key == -1) {
        return -1;
    }
    return semget(key, 1, 0);
}

void close_semaphore(int sem) {}

void unlink_semaphore(const char* filename) {
    int semid = open_semaphore(filename);
    if( semid == -1) {
        fprintf(stderr, "[ERROR] Failed to unlink semaphore.\n");
        return;
    }
    semctl(semid, 0, IPC_RMID);
}

void aquire(int sem) {
    struct sembuf operation = { 0, -1, 0 };
    if(semop(sem, &operation, 1) == -1) {
        perror("aquire"); 
    }
}

void release(int sem) {
    struct sembuf operation = { 0, 1, 0 };
    if(semop(sem, &operation, 1) == -1){
        perror("aquire");
    }
}