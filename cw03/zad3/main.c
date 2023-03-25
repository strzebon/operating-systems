#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char exe_path[PATH_MAX];
char full_path[PATH_MAX];

int main(int argc, char** argv){
    if(argc != 3){
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    if(strlen(argv[2]) > 255){
        fprintf(stderr, "Too long argv[2]\n");
        return 2;
    }

    DIR* dir = opendir(argv[1]);
    if(dir == NULL){
        fprintf(stderr, "opendir error\n");
        return 3;
    }

    realpath(argv[0], exe_path);
    struct dirent* object;
    struct stat st;

    while ((object = readdir(dir))){
        if(strcmp(object->d_name, ".") == 0 || strcmp(object->d_name, "..") == 0) continue;

        snprintf(full_path, PATH_MAX, "%s/%s", argv[1], object->d_name);
        stat(full_path, &st);

        if(S_ISDIR(st.st_mode)){
            if(fork() == 0) execl(exe_path, "main", full_path, argv[2], NULL);
        }
        else{
            FILE *file = fopen(full_path, "r");
            char *buffer = calloc(strlen(argv[2]), sizeof(char));
            fread(buffer, sizeof(char), strlen(argv[2]), file);
            if(strcmp(buffer, argv[2]) == 0){
                printf("%s %d\n", full_path, getpid());
                fflush(NULL);
            }
            free(buffer);
        }
    }

    while(wait(NULL) > 0);
    return 0;
}