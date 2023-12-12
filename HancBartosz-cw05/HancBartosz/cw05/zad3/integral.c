#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_CHILDREN 100
#define BUFF_SIZE 2048

clock_t start, end;
double cpu_time_used;
double integral_value = 0.0;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        puts("Incorrect number of arguments!");
        return 1;
    }

    int n_children = atoi(argv[2]);
    char *pipe_path = "/tmp/my_pipe";

    if (n_children > MAX_CHILDREN)
    {
        printf("Too many children. Max: %d\n", MAX_CHILDREN);
        return 1;
    }

    if (mkfifo(pipe_path, 0666))
    {
        perror("mkfifo");
        return 1;
    }

    start = clock();

    double a = 0.0;
    double b = 1.0;
    double h = (b - a) / n_children;
    double partial_integral, local_a, local_b;

    for (int i = 0; i < n_children; i++)
    {
        local_a = a + i * h;
        local_b = local_a + h;
        char arg_a[BUFF_SIZE];
        char arg_b[BUFF_SIZE];
        snprintf(arg_a, BUFF_SIZE, "%lf", local_a);
        snprintf(arg_b, BUFF_SIZE, "%lf", local_b);

        if (!fork())
        {
            execl("./partial_integral.exe", "partial_integral.exe", pipe_path, arg_a, arg_b, argv[1], NULL);
        }
        else
        {
            int fd = open(pipe_path, O_RDONLY);
            wait(NULL);
            read(fd, &partial_integral, sizeof(double));
            integral_value += partial_integral;
            close(fd);
        }
    }

    remove(pipe_path);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Integral value: %.10f | Time: %.5f s\n", integral_value, cpu_time_used);

    return 0;
}