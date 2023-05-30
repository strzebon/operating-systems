#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "lib.h"

#define CLIENTS_MAX 15

typedef struct{
    int fd;
    char name[TEXT_SIZE];
    int active;
} Client;

typedef struct{
    enum type {socket_event, client_event} type;
    Client* client;
    int socket;
} Event_data;

Client clients[CLIENTS_MAX];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll_fd;

void delete_client(Client* client){
    client->active = -1;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
    printf("Deleted %s\n", client->name);
}

void init_socket(int socket, void* addr, int len){
    bind(socket, (struct sockaddr*) addr, len);
    listen(socket, CLIENTS_MAX);
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    Event_data* event_data = malloc(sizeof(Event_data));
    event_data->type = socket_event;
    event_data->socket = socket;
    event.data.ptr = event_data;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

void* ping(void* arg){
    Msg msg;
    msg.type = PING;

    while(1){
        sleep(30);
        pthread_mutex_lock(&mutex);
        printf("Pinging\n");
        for(int i=0; i<CLIENTS_MAX; i++){
            if(clients[i].active == -1) continue;
            if(clients[i].active == 0) delete_client(&clients[i]);
            else{
                clients[i].active = 0;
                write(clients[i].fd, &msg, sizeof(Msg));
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

Client* create_client(int fd){
    pthread_mutex_lock(&mutex);
    Client* client = NULL;
    for(int i=0; i<CLIENTS_MAX; i++){
        if(clients[i].active == -1){
            client = &clients[i];
            break;
        }
    }

    if(client == NULL) return NULL;

    client->fd = fd;
    client->active = -1;
    pthread_mutex_unlock(&mutex);
    return client;
}

void handle_client_message(Client* client){
    if (client->active == -1) {
        int name_size = read(client->fd, client->name, sizeof(client->name) - 1);
        pthread_mutex_lock(&mutex);
        int flag = 0;
        for(int i=0; i<CLIENTS_MAX; i++) {
            if(clients[i].active != -1 && strcmp(clients[i].name, client->name) == 0){
                flag = 1;
                break;
            }
        }
        if(flag){
            Msg msg;
            msg.type = NAME_TAKEN;
            write(client->fd, &msg, sizeof(msg));
            delete_client(client);
        }
        else{
            client->active = 1;
            client->name[name_size] = '\0';
            printf("New client %s\n", client->name);
        }

        pthread_mutex_unlock(&mutex);
    }
    else{
        Msg msg;
        read(client->fd, &msg, sizeof(Msg));

        if(msg.type == PING){
            pthread_mutex_lock(&mutex);
            printf("%s\n", client->name);
            client->active = 1;
            pthread_mutex_unlock(&mutex);
        }
        else if(msg.type == STOP){
            pthread_mutex_lock(&mutex);
            delete_client(client);
            pthread_mutex_unlock(&mutex);
        }
        else if(msg.type == TO_ALL){
            char text[TEXT_SIZE] = "";
            strcat(text, client->name);
            strcat(text, ": ");
            strcat(text, msg.text);

            for(int i=0; i<CLIENTS_MAX; i++){
                if (clients[i].active != -1 && &clients[i] != client){
                    Msg msg;
                    msg.type = GET;
                    memcpy(&msg.text, text, TEXT_SIZE * sizeof(char));
                    write(clients[i].fd, &msg, sizeof(msg));
                }
            }
        }
        else if(msg.type == LIST) {
            for(int i=0; i<CLIENTS_MAX; i++){
                if(clients[i].active != -1){
                    Msg msg;
                    msg.type = GET;
                    memcpy(&msg.text, clients[i].name, TEXT_SIZE * sizeof(char));
                    write(client->fd, &msg, sizeof(msg));
                }
            }
        }
        else if(msg.type == TO_ONE){
            char text[TEXT_SIZE] = "";
            strcat(text, client->name);
            strcat(text, ": ");
            strcat(text, msg.text);

            for(int i=0; i<CLIENTS_MAX; i++){
                if(clients[i].active != -1) {
                    if(strcmp(clients[i].name, msg.receiver) == 0) {
                        Msg msg;
                        msg.type = GET;
                        memcpy(&msg.text, text, TEXT_SIZE * sizeof(char));
                        write(clients[i].fd, &msg, sizeof(msg));
                        break;
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv){
    if(argc != 3){
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    for(int i=0; i<CLIENTS_MAX; i++){
        clients[i].active = -1;
    }

    int port = atoi(argv[1]);
    char* socket_path = argv[2];
    epoll_fd = epoll_create1(0);

    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strncpy(local_addr.sun_path, socket_path, sizeof(local_addr.sun_path));

    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(port);
    web_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    unlink(socket_path);
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    init_socket(local_socket, &local_addr, sizeof(local_addr));

    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    init_socket(web_socket, &web_addr, sizeof(web_addr));

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);
    printf("Server started\n");
    struct epoll_event events[10];

    while(1){
        int n = epoll_wait(epoll_fd, events, 10, -1);
        for(int i=0; i<n; i++){
            Event_data* data = events[i].data.ptr;
            if(data->type == socket_event){
                int client_fd = accept(data->socket, NULL, NULL);
                Client* client = create_client(client_fd);
                if(client == NULL){
                    Msg msg;
                    msg.type = SERVER_FULL;
                    write(client_fd, &msg, sizeof(Msg));
                    close(client_fd);
                    continue;
                }
                Event_data* event_data = malloc(sizeof(event_data));
                event_data->type = client_event;
                event_data->client = client;
                struct epoll_event event = { .events = EPOLLIN | EPOLLET | EPOLLHUP, .data = { event_data } };
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            } else if (data->type == client_event) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    delete_client(data->client);
                    pthread_mutex_unlock(&mutex);
                } else handle_client_message(data->client);
            }
        }
    }
}