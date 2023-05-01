#include <string.h>

#include "queue.h"
#include "library.h"

char queue_pop(char *queue) {
    char byte = queue[0];
    memcpy(queue, queue + 1, strlen(queue) + 1);
    return byte;
}

void queue_push(char *queue, char byte) {
    int size = strlen(queue);
    queue[size] = byte;
    queue[size + 1] = '\0';
}

bool queue_full(char *queue) {
    return (strlen(queue) + 1 == BUFFER_SIZE);
}

bool queue_empty(char *queue) {
    return (strlen(queue) == 0);
}