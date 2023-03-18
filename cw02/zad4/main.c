#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>
#include <errno.h>
#include <string.h>

long long total_size = 0;

int count_file(const char *file_path, const struct stat *st, int flag) {
    if(flag == FTW_D) return 0;
    struct stat statistics;
    stat(file_path, &statistics);
    printf("%ld %s\n", statistics.st_size, file_path);
    total_size += statistics.st_size;
    return 0;
}

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    char* dir_name = argv[1];
    if(ftw(dir_name, count_file, 1) == -1){
        fprintf(stderr, "ftw error");
    }

    printf("%lld total\n", total_size);
    return 0;
}