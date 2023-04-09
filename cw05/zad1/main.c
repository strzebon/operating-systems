#include <stdio.h>
#include <string.h>

#define MAX_SIZE 4096

int main(int argc, char** argv){
    if(argc == 2){
        if(strcmp(argv[1], "data") == 0){
            FILE* fp = popen("mail | tail -n +2", "w");
            fputs("exit", fp);
            pclose(fp);
        }
        else if(strcmp(argv[1], "nadawca") == 0){
            FILE *fp = popen("mail | tail -n +2 | sort -k 3", "w");
            fputs("exit", fp);
            pclose(fp);
        }
        else{
            fprintf(stderr, "Invalid arguments\n");
            return 1;
        }
    }
    else if(argc == 4){
        char command[MAX_SIZE];
        sprintf(command, "echo %s | mail -s %s %s", argv[3], argv[2], argv[1]);
        FILE* fp = popen(command, "r");
        pclose(fp);
    }
    else{
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    return 0;
}