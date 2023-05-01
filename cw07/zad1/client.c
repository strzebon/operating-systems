#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "shared_memory.h"
#include "queue.h"
#include "library.h"
#include "semaphore.h"

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int buffer_mutex;

void open_semaphores() {
    sem_queue = open_semaphore(SEM_QUEUE_FNAME);
    sem_chairs = open_semaphore(SEM_CHAIRS_FNAME);
    sem_barbers = open_semaphore(SEM_BARBERS_FNAME);
    buffer_mutex = open_semaphore(SEM_BUFFER_MUTEX_FNAME);
}

char random_haircut() {
    return (char) (rand() % 128);
}

int main(void) {
    srand(time(NULL) + getpid());

    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL) {
        fprintf(stderr, "Couldn't open queue.\n");
        exit(1);
    }

    open_semaphores();

    if (strlen(queue) >= QUEUE_SIZE) {
        printf("CLIENT %d: Full queue. Leaving.\n", getpid());
        fflush(stdout);
    }
    
    aquire(sem_queue);

    aquire(buffer_mutex);
    char haircut = random_haircut();
    printf("CLIENT %d: New client with haircut %d\n", getpid(), haircut);
    fflush(stdout);
    queue_push(queue, haircut);
    release(buffer_mutex);
    
    release(sem_barbers);
    aquire(sem_chairs);

    printf("CLIENT %d: Done.\n", getpid());
    fflush(stdout);

    detach_shared_memory(queue);

    return 0;
}