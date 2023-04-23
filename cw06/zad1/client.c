#include "library.h"

key_t server_key;
int server_queue;
key_t key;
int queue;
int id;

void stop(){
    MsgBuf *msgBuf = malloc(sizeof(MsgBuf));
    time_t t = time(NULL);
    msgBuf->time = *localtime(&t);
    msgBuf->type = STOP;
    msgBuf->client_id = id;

    msgsnd(server_queue, msgBuf, sizeof(MsgBuf), 0);
    msgctl(queue, IPC_RMID, NULL);
    exit(0);
}

void init(){
    MsgBuf *msgBuf = malloc(sizeof(MsgBuf));
    time_t t = time(NULL);
    msgBuf->time = *localtime(&t);
    msgBuf->type = INIT;
    msgBuf->key = key;
    msgsnd(server_queue, msgBuf, sizeof(MsgBuf), 0);
    msgrcv(queue, msgBuf, sizeof(MsgBuf), 0, 0);

    id = msgBuf->client_id;
    free(msgBuf);
    if(id == -1){
        printf("Max number of clients reached\n");
        exit(0);
    }
    signal(SIGINT, stop);
}

void read_messages(){
    MsgBuf *msgBuf = malloc(sizeof(MsgBuf));
    while(msgrcv(queue, msgBuf, sizeof(MsgBuf), 0, IPC_NOWAIT) >= 0){
        if(msgBuf->type == STOP){
            printf("Received STOP message\n");
            stop();
        }
        else{
            struct tm t = msgBuf->time;
            printf("Received message:\n%02d:%02d:%02d\nfrom: %d\ntext: %s\n",
                   t.tm_hour, t.tm_min, t.tm_sec, msgBuf->client_id, msgBuf->text);
        }
    }
    free(msgBuf);
}

void list(){
    MsgBuf* msgBuf = malloc(sizeof(MsgBuf));
    time_t t = time(NULL);
    msgBuf->time = *localtime(&t);
    msgBuf->type = LIST;
    msgBuf->client_id = id;

    msgsnd(server_queue, msgBuf, sizeof(MsgBuf), 0);
    msgrcv(queue, msgBuf, sizeof(MsgBuf), 0, 0);
    printf("%s\n", msgBuf->text);
    free(msgBuf);
}

void to_all(char* message){
    MsgBuf* msgBuf = malloc(sizeof(MsgBuf));
    time_t t = time(NULL);
    msgBuf->time = *localtime(&t);
    msgBuf->type = ALL;
    msgBuf->client_id = id;
    strcpy(msgBuf->text, message);

    msgsnd(server_queue, msgBuf, sizeof(MsgBuf), 0);
}

void to_one(int receiver, char* message){
    MsgBuf* msgBuf = malloc(sizeof(MsgBuf));
    time_t t = time(NULL);
    msgBuf->time = *localtime(&t);
    msgBuf->type = ONE;
    msgBuf->client_id = id;
    msgBuf->receiver = receiver;
    strcpy(msgBuf->text, message);

    msgsnd(server_queue, msgBuf, sizeof(MsgBuf), 0);
}

int main(){
    server_key = ftok(HOME, SERVER_ID);
    server_queue = msgget(server_key, 0);
    srand(time(NULL));
    key = ftok(HOME, rand() % 100 + 1);
    queue = msgget(key, IPC_CREAT | 0666);

    init();
    char *buf;
    size_t x;
    size_t size;
    while(1){
        printf(">>");
        size = getline(&buf, &x, stdin);
        buf[size-1] = '\0';
        read_messages();

        if (strcmp(buf, "") == 0) {
            continue;
        }
        char* command = strtok(buf, " ");
        if(strcmp(command, "LIST") == 0) list();
        else if(strcmp(command, "2ALL") == 0){
            char *message = strtok(NULL, "\0");
            to_all(message);
        }
        else if(strcmp(command, "2ONE") == 0){
            char *str = strtok(NULL, " ");
            int receiver = atoi(str);
            char *message = strtok(NULL, "\0");
            to_one(receiver, message);
        }
        else if(strcmp(command, "STOP") == 0) stop();
        else printf("unknown command\n");
    }
}