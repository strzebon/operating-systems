#include "library.h"

int no_clients = 0;
int queue;
int queues[MAX_NO_CLIENTS];
FILE* logs;

void handler(int signo){
    MsgBuf* msgBuf = malloc(sizeof(MsgBuf));
    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == -1) continue;
        msgBuf->type = STOP;
        msgsnd(queues[i], msgBuf, sizeof(MsgBuf), 0);
        msgrcv(queue, msgBuf, sizeof(MsgBuf), STOP, 0);
    }

    free(msgBuf);
    msgctl(queue, IPC_RMID, NULL);
    fclose(logs);
    exit(0);
}

void init(MsgBuf* msgBuf){
    struct tm t = msgBuf->time;
    fprintf(logs, "%02d:%02d:%02d INIT Client ID: ", t.tm_hour, t.tm_min, t.tm_sec);

    if(no_clients == MAX_NO_CLIENTS){
        msgBuf->client_id = -1;
        fprintf(logs, "max number of clients reached\n\n");
    }
    else{
        int id;
        for(id=0; id<MAX_NO_CLIENTS; id++){
            if(queues[id] == -1) break;
        }
        msgBuf->client_id = id;
        queues[id] = msgget(msgBuf->key, 0);
        no_clients++;
        fprintf(logs, "%d\n\n", id);
    }

    msgsnd(msgget(msgBuf->key, 0), msgBuf, sizeof(MsgBuf), 0);
}

void list(MsgBuf *msgBuf){
    struct tm t = msgBuf->time;
    fprintf(logs, "%02d:%02d:%02d LIST Client ID: %d\n\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf->client_id);
    strcpy(msgBuf->text, "");

    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == -1) continue;
        sprintf(msgBuf->text + strlen(msgBuf->text), "%d ", i);
    }

    msgsnd(queues[msgBuf->client_id], msgBuf, sizeof(MsgBuf), 0);
}

void to_all(MsgBuf* msgBuf){
    struct tm t = msgBuf->time;
    fprintf(logs, "%02d:%02d:%02d 2ALL Client ID: %d\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf->client_id);
    fprintf(logs, "%s\n\n", msgBuf->text);

    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == -1 || i == msgBuf->client_id) continue;
        msgsnd(queues[i], msgBuf, sizeof(MsgBuf), 0);
    }
}

void to_one(MsgBuf* msgBuf){
    struct tm t = msgBuf->time;
    fprintf(logs, "%02d:%02d:%02d 2ONE Client ID: %d\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf->client_id);
    fprintf(logs, "%s\n\n", msgBuf->text);

    msgsnd(queues[msgBuf->receiver], msgBuf, sizeof(MsgBuf), 0);
}

void stop(MsgBuf* msgBuf){
    struct tm t = msgBuf->time;
    fprintf(logs, "%02d:%02d:%02d STOP Client ID: %d\n\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf->client_id);
    queues[msgBuf->client_id] = -1;
    no_clients--;
}

int main(){
    for(int i=0; i<MAX_NO_CLIENTS; i++){
        queues[i] = -1;
    }
    logs = fopen("logs.txt", "a");
    key_t key = ftok(HOME, SERVER_ID);
    queue = msgget(key, IPC_CREAT | 0666);
    signal(SIGINT, handler);

    MsgBuf* msgBuf = malloc(sizeof(MsgBuf));
    while(1){
        msgrcv(queue, msgBuf, sizeof(MsgBuf), -6, 0);
        switch(msgBuf->type){
            case INIT:
                init(msgBuf);
                break;
            case LIST:
                list(msgBuf);
                break;
            case ALL:
                to_all(msgBuf);
                break;
            case ONE:
                to_one(msgBuf);
                break;
            case STOP:
                stop(msgBuf);
                break;
            default:
                printf("Unknown type\n");
        }
    }
}