#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    char* dir_name = argv[1];
    DIR* dir = opendir(dir_name);
    if(dir == NULL){
        fprintf(stderr, "opendir error");
        return 2;
    }

    struct dirent* object;
    struct stat statistics;
    long long total_size = 0;

    while ((object = readdir(dir))){
        stat(object->d_name, &statistics);
        if(!S_ISDIR(statistics.st_mode)){
            printf("%ld %s\n", statistics.st_size, object->d_name);
            total_size += statistics.st_size;
        }
    }

    printf("%lld total\n", total_size);
    return 0;
}