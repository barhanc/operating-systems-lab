#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;
const int N = grid_width * grid_height;

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

// Multithreading

pthread_t *threads;
int no_threads;
bool initialized = false;

typedef struct thread_info
{
    char *src;
    char *dst;
    int block_s;
    int block_e;
} thread_info;

void *thread_start(void *arg)
{
    thread_info *args = arg;

    while (true)
    {
        for (int i = args->block_s; i < args->block_e; i++)
        {
            args->dst[i] = is_alive(i / grid_width, i % grid_width, args->src);
        }

        pause();

        char *tmp = args->src;
        args->src = args->dst;
        args->dst = tmp;
    }
}

void sighandler(int signo) {}

void update_grid(char *src, char *dst, int thread_opt)
{
    if (!initialized)
    {
        signal(SIGUSR1, sighandler);
        no_threads = thread_opt > 0 && thread_opt < N ? thread_opt : N;
        threads = malloc(no_threads * sizeof(pthread_t));

        int block_size = N / no_threads;

        for (int i = 0; i < N; i += block_size)
        {
            thread_info *args = malloc(sizeof(thread_info));
            args->src = src;
            args->dst = dst;
            args->block_s = i;
            args->block_e = (i + 2 * block_size > N) ? N : i + block_size;

            pthread_create(&threads[i / block_size], NULL, thread_start, (void *)args);
        }

        initialized = true;
    }

    for (int i = 0; i < no_threads; i++)
    {
        pthread_kill(threads[i], SIGUSR1);
    }
}