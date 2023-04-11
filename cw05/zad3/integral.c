#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


double integral(double h, double a, double b){
    double s = 0;
    double x;
    for(x=a; x+h<b; x+=h){
        s += h * 4/((x + 0.5*h)*(x + 0.5*h) + 1);
    }
    h = b-x;
    s += h * 4/((x + 0.5*h)*(x + 0.5*h) + 1);
    return s;
}

int main(int argc, char** argv){
    double h = atof(argv[1]);
    double a = atof(argv[2]);
    double b = atof(argv[3]);
    char buf[100];
    char* path = "/tmp/fifo";

    sprintf(buf, "%lf,", integral(h, a, b));
    int fd = open(path, O_WRONLY);
    write(fd, buf, 100);
    close(fd);

    return 0;
}