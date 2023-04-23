#include <sys/msg.h>
#include <sys/ipc.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define MAX_NO_CLIENTS 10
#define MAX_MSG_SIZE 1024
#define SERVER_NAME "/SERVER"

typedef struct MsgBuf{
    long type;
    char text[MAX_MSG_SIZE];
    int client_id;
    int receiver;
    struct tm time;
} MsgBuf;

typedef enum MsgType {
    INIT = 1,
    LIST = 2,
    ALL = 3,
    ONE = 4,
    STOP = 5
} MsgType;