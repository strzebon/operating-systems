#include <semaphore.h>

#define PROJECT_IDENTIFIER getenv("HOME")
#define SEM_QUEUE_FNAME "0"
#define SEM_CHAIRS_FNAME "1"
#define SEM_BARBERS_FNAME "2"
#define SEM_BUFFER_MUTEX_FNAME "3"

#define BARBER_TOTAL 3
#define CHAIR_TOTAL 2
#define QUEUE_SIZE 5
#define CUSTOMERS_TOTAL 7
#define BUFFER_SIZE 4096