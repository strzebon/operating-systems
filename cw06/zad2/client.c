#include "library.h"

char name[3];
mqd_t mqdes;
mqd_t server_mqdes;
int id;

void stop(){
    MsgBuf msgBuf;
    time_t t = time(NULL);
    msgBuf.time = *localtime(&t);
    msgBuf.type = STOP;
    msgBuf.client_id = id;

    mq_send(server_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_close(server_mqdes);
    exit(0);
}

void init(){
    MsgBuf msgBuf;
    time_t t = time(NULL);
    msgBuf.time = *localtime(&t);
    msgBuf.type = INIT;
    strcpy(msgBuf.text, name);

    mq_send(server_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_receive(mqdes, (char*) &msgBuf, sizeof(MsgBuf), NULL);

    id = msgBuf.client_id;
    if(id == -1){
        printf("Max number of clients reached\n");
        exit(0);
    }
    signal(SIGINT, stop);
}

void read_messages(){
    MsgBuf msgBuf;
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    t.tv_sec += 0.1;
    while(mq_timedreceive(mqdes, (char *) &msgBuf, sizeof(MsgBuf), NULL, &t) != -1){
        if(msgBuf.type == STOP){
            printf("Received STOP message\n");
            stop();
        }
        else{
            struct tm t = msgBuf.time;
            printf("Received message:\n%02d:%02d:%02d\nfrom: %d\ntext: %s\n",
                   t.tm_hour, t.tm_min, t.tm_sec, msgBuf.client_id, msgBuf.text);
        }
    }
}

void list(){
    MsgBuf msgBuf;
    time_t t = time(NULL);
    msgBuf.time = *localtime(&t);
    msgBuf.type = LIST;
    msgBuf.client_id = id;

    mq_send(server_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
    mq_receive(mqdes, (char*) &msgBuf, sizeof(MsgBuf), NULL);

    printf("%s\n", msgBuf.text);
}

void to_all(char* message){
    MsgBuf msgBuf;
    time_t t = time(NULL);
    msgBuf.time = *localtime(&t);
    msgBuf.type = ALL;
    msgBuf.client_id = id;
    strcpy(msgBuf.text, message);

    mq_send(server_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
}

void to_one(int receiver, char* message){
    MsgBuf msgBuf;
    time_t t = time(NULL);
    msgBuf.time = *localtime(&t);
    msgBuf.type = ONE;
    msgBuf.client_id = id;
    msgBuf.receiver = receiver;
    strcpy(msgBuf.text, message);

    mq_send(server_mqdes, (char*) &msgBuf, sizeof(MsgBuf), 0);
}

int main(){
    srand(time(NULL));
    name[0] = '/';
    name[1] = rand() % ('Z' - 'A' + 1) + 'A';
    name[2] = rand() % ('Z' - 'A' + 1) + 'A';
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_NO_CLIENTS;
    attr.mq_msgsize = sizeof(MsgBuf);
    mqdes = mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
    server_mqdes = mq_open(SERVER_NAME, O_RDWR);
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