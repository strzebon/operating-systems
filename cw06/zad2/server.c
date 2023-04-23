#include "library.h"

int no_clients = 0;
mqd_t mqdes;
char* queues[MAX_NO_CLIENTS];
FILE* logs;

void handler(int signo){
    MsgBuf msgBuf;
    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == NULL) continue;
        msgBuf.type = STOP;
        mqd_t client_mqdes = mq_open(queues[i], O_RDWR);
        mq_send(client_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
        mq_receive(mqdes, (char*) &msgBuf, sizeof(MsgBuf), NULL);
        mq_close(client_mqdes);
    }

    mq_close(mqdes);
    fclose(logs);
    exit(0);
}

void init(MsgBuf msgBuf){
    struct tm t = msgBuf.time;
    fprintf(logs, "%02d:%02d:%02d INIT Client ID: ", t.tm_hour, t.tm_min, t.tm_sec);

    if(no_clients == MAX_NO_CLIENTS){
        msgBuf.client_id = -1;
        fprintf(logs, "max number of clients reached\n\n");
    }
    else{
        int id;
        for(id=0; id<MAX_NO_CLIENTS; id++){
            if(queues[id] == NULL) break;
        }
        msgBuf.client_id = id;
        queues[id] = (char*) calloc(3, sizeof(char));
        strcpy(queues[id], msgBuf.text);
        no_clients++;
        fprintf(logs, "%d\n\n", id);
    }

    mqd_t client_mqdes = mq_open(msgBuf.text, O_RDWR);
    mq_send(client_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_close(client_mqdes);
}

void list(MsgBuf msgBuf){
    struct tm t = msgBuf.time;
    fprintf(logs, "%02d:%02d:%02d LIST Client ID: %d\n\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf.client_id);
    strcpy(msgBuf.text, "");

    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == NULL) continue;
        sprintf(msgBuf.text + strlen(msgBuf.text), "%d ", i);
    }

    mqd_t client_mqdes = mq_open(queues[msgBuf.client_id], O_RDWR);
    mq_send(client_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_close(client_mqdes);
}

void to_all(MsgBuf msgBuf){
    struct tm t = msgBuf.time;
    fprintf(logs, "%02d:%02d:%02d 2ALL Client ID: %d\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf.client_id);
    fprintf(logs, "%s\n\n", msgBuf.text);

    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if(queues[i] == NULL || i == msgBuf.client_id) continue;
        mqd_t client_mqdes = mq_open(queues[i], O_RDWR);
        mq_send(client_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
        mq_close(client_mqdes);
    }
}

void to_one(MsgBuf msgBuf){
    struct tm t = msgBuf.time;
    fprintf(logs, "%02d:%02d:%02d 2ONE Client ID: %d\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf.client_id);
    fprintf(logs, "%s\n\n", msgBuf.text);

    mqd_t client_mqdes = mq_open(queues[msgBuf.receiver], O_RDWR);
    mq_send(client_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_close(client_mqdes);}

void stop(MsgBuf msgBuf){
    struct tm t = msgBuf.time;
    fprintf(logs, "%02d:%02d:%02d STOP Client ID: %d\n\n", t.tm_hour, t.tm_min, t.tm_sec, msgBuf.client_id);
    queues[msgBuf.client_id] = NULL;
    no_clients--;
}

int main(){
    for(int i=0; i<MAX_NO_CLIENTS; i++){
        queues[i] = NULL;
    }
    logs = fopen("logs.txt", "a");
    mq_unlink(SERVER_NAME);
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_NO_CLIENTS;
    attr.mq_msgsize = sizeof(MsgBuf);
    mqdes = mq_open(SERVER_NAME, O_RDWR | O_CREAT, 0666, &attr);
    signal(SIGINT, handler);
    MsgBuf msgBuf;

    while(1){
        mq_receive(mqdes, (char*) &msgBuf, sizeof(MsgBuf), NULL);
        switch(msgBuf.type){
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