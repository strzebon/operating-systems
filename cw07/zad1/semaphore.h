#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>

int create_semaphore(const char *filename, int initial);
int open_semaphore(const char *filename);
void close_semaphore(int sem);
void unlink_semaphore(const char* filename);
void aquire(int sem);
void release(int sem);