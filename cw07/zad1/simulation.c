#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "library.h"
#include "shared_memory.h"
#include "semaphore.h"

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int buffer_mutex;

void close_semaphores() {
    close_semaphore(sem_queue);
    close_semaphore(sem_chairs);
    close_semaphore(sem_barbers);
    close_semaphore(buffer_mutex);
}

void unlink_semaphores() {
    unlink_semaphore(SEM_QUEUE_FNAME);
    unlink_semaphore(SEM_CHAIRS_FNAME);
    unlink_semaphore(SEM_BARBERS_FNAME);
    unlink_semaphore(SEM_BUFFER_MUTEX_FNAME);
}

void create_semaphores() {
    sem_queue =  create_semaphore(SEM_QUEUE_FNAME, CHAIR_TOTAL);
    sem_chairs =  create_semaphore(SEM_CHAIRS_FNAME, 0);
    sem_barbers =  create_semaphore(SEM_BARBERS_FNAME, 0);
    buffer_mutex =  create_semaphore(SEM_BUFFER_MUTEX_FNAME, 1);
}

int main() {
    printf("barbers: %d, chairs: %d, queue size: %d, customers: %d\n", BARBER_TOTAL, CHAIR_TOTAL, QUEUE_SIZE, CUSTOMERS_TOTAL);
    fflush(stdout);

    char *shared = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if(shared == NULL) {
        exit(EXIT_FAILURE);
    }
    shared[0] = '\0';

    unlink_semaphores();
    create_semaphores();

    for(int i=0;i<BARBER_TOTAL;++i)
        if (fork() == 0)
            execl("./barber", "./barber", NULL);

    for(int i=0;i<CUSTOMERS_TOTAL;++i)
        if (fork() == 0)
            execl("./client", "./client", NULL);

    while(wait(NULL) > 0);

    if (!destroy_shared_memory(PROJECT_IDENTIFIER)) {
        fprintf(stderr, "Failed to release shared memory.\n");
        exit(1);
    }
    close_semaphores();
    printf("Simulation finished.\n");
    fflush(stdout);

    return 0;
}