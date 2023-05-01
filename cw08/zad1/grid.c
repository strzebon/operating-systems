#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;
const int n = grid_width * grid_height;
pthread_t *threads;

typedef struct{
    char *src;
    char *dst;
    int square;
} Args;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void handler(int signo){}

void update_grid(){
    for(int i=0; i<n; i++){
        pthread_kill(threads[i], SIGUSR1);
    }
}

void *update_square(void *_args){
    Args *args = (Args*) _args;
    int i = (args->square) / grid_width;
    int j = (args->square) % grid_width;

    while(true){
        args->dst[args->square] = is_alive(i, j, args->src);
        pause();

        char *tmp = args->src;
        args->src = args->dst;
        args->dst = tmp;
    }

    return NULL;
}

void create_threads(char *src, char *dst){
    signal(SIGUSR1, handler);
    threads = malloc(n * sizeof(pthread_t));

    for(int i=0; i<n; i++){
        Args *args = malloc(sizeof(Args));
        args->src = src;
        args->dst = dst;
        args->square = i;

        pthread_create(&threads[i], NULL, update_square, (void*) args);
    }
}