#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "lib.h"

int sock;

int connect_unix(char* path) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) -1);
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*) &addr, sizeof(addr));
    return sock;
}

int connect_web(char* ipv4, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
        puts("Invalid address\n");
        exit(0);
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*) &addr, sizeof addr);
    return sock;
}

void on_SIGINT(int signo) {
    Msg msg;
    msg.type = STOP;
    write(sock, &msg, sizeof(msg));
    exit(0);
}

int main(int argc, char** argv) {
    if (strcmp(argv[2], "web") == 0 && argc == 5) sock = connect_web(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4) sock = connect_unix(argv[3]);
    else {
        puts("Invalid arguments\n");
        exit(0);
    }

    signal(SIGINT, on_SIGINT);
    char* name = argv[1];
    write(sock, name, strlen(name));

    int epoll_fd = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = STDIN_FILENO;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event);

    struct epoll_event socket_event;
    socket_event.events = EPOLLIN | EPOLLPRI | EPOLLHUP;
    socket_event.data.fd = sock;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &socket_event);

    struct epoll_event events[2];

    while(1){
        int n = epoll_wait(epoll_fd, events, 2, 1);
        for(int i=0; i<n; i++){
            if (events[i].data.fd == STDIN_FILENO) {
                char buffer[512] = {};
                int x = read(STDIN_FILENO, &buffer, 512);
                buffer[x] = 0;

                char *token;
                token = strtok(buffer, " \n");

                Type type = -1;
                char receiver[TEXT_SIZE] = {};
                char text[TEXT_SIZE] = {};


                if(token == NULL) continue;
                if(strcmp(token, "LIST") == 0) type = LIST;
                else if(strcmp(token, "2ALL") == 0){
                    type = TO_ALL;
                    token = strtok(NULL, "\n");
                    memcpy(text, token, strlen(token)*sizeof(char));
                    text[strlen(token)] = 0;
                }
                else if(strcmp(token, "2ONE") == 0){
                    type = TO_ONE;
                    token = strtok(NULL, " ");
                    memcpy(receiver, token, strlen(token)*sizeof(char));
                    receiver[strlen(token)] = 0;

                    token = strtok(NULL, "\n");
                    memcpy(text, token, strlen(token)*sizeof(char));
                    text[strlen(token)] = 0;
                }
                else if(strcmp(token, "STOP") == 0) type = STOP;
                else{
                    puts("Invalid command");
                    continue;
                }

                Msg msg;
                msg.type = type;
                memcpy(&msg.receiver, receiver, strlen(receiver) + 1);
                memcpy(&msg.text, text, strlen(text) + 1);

                write(sock, &msg, sizeof(msg));
            }
            else{
                Msg msg;
                read(sock, &msg, sizeof msg);

                if(msg.type == NAME_TAKEN){
                    puts("This name is taken\n");
                    close(sock);
                    exit(0);
                }
                else if(msg.type == SERVER_FULL){
                    puts("Server is full\n");
                    close(sock);
                    exit(0);
                }
                else if(events[i].events & EPOLLHUP){
                    puts("Disconnected\n");
                    exit(0);
                }
                else if(msg.type == PING){
                    write(sock, &msg, sizeof(msg));
                }
                else if(msg.type == STOP){
                    close(sock);
                    exit(0);
                }
                else if(msg.type == GET){
                    puts(msg.text);
                }
            }


        }
    }
}