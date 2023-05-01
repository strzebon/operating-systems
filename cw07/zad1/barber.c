#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "library.h"
#include "shared_memory.h"
#include "queue.h"
#include "semaphore.h"

#define HAIRCUT_TIME 1000
#define TIMEOUT 1000000

static int sem_queue;
static int sem_chairs;
static int sem_barbers;
static int buffer_mutex;

void open_semaphores()
{
    sem_queue = open_semaphore(SEM_QUEUE_FNAME);
    sem_chairs = open_semaphore(SEM_CHAIRS_FNAME);
    sem_barbers = open_semaphore(SEM_BARBERS_FNAME);
    buffer_mutex = open_semaphore(SEM_BUFFER_MUTEX_FNAME);
}

int main()
{
    char *queue = attach_shared_memory(PROJECT_IDENTIFIER, BUFFER_SIZE);
    if (queue == NULL)
    {
        fprintf(stderr, "Couldn't open queue.\n");
        exit(1);
    }
    open_semaphores();

    printf("BARBER %d: Spawned\n", getpid());
    fflush(stdout);

    while (true)
    {

        aquire(sem_barbers);

        release(buffer_mutex);
        char haircut = queue_pop(queue);
        release(buffer_mutex);

        printf("BARBER %d: Started haircut %d\n", getpid(), haircut);
        fflush(stdout);

        usleep(HAIRCUT_TIME);

        printf("BARBER %d: Finished haircut %d\n", getpid(), haircut);
        fflush(stdout);

        release(sem_chairs);
        release(sem_queue);

        if (queue_empty(queue))
        {
            usleep(TIMEOUT);
            if (queue_empty(queue)) break;
        }
    }
    printf("BARBER %d: Empty queue. Closing.\n", getpid());
    fflush(stdout);
    detach_shared_memory(queue);

    return 0;
}