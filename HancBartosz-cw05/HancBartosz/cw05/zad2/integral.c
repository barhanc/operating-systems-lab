#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_CHILDREN 100

clock_t start, end;
double cpu_time_used;
double integral_value = 0.0;

double f(double x)
{
    return 4.0 / (x * x + 1.0);
}

double integrate(double a, double b, double width)
{
    double integral_value = 0.0;
    double x;

    for (x = a + 0.5 * width; x < b; x += width)
    {
        integral_value += f(x) * width;
    }

    return integral_value;
}

int main(int argc, char *argv[])
{
    int pipefd[2], status;
    pid_t pid[MAX_CHILDREN];

    if (argc != 3)
    {
        puts("Incorrect number of arguments!");
        return 1;
    }

    double width = atof(argv[1]);
    int n_children = atoi(argv[2]);

    if (n_children > MAX_CHILDREN)
    {
        printf("Too many children. Max: %d\n", MAX_CHILDREN);
        return 1;
    }

    start = clock();

    double a = 0.0;
    double b = 1.0;
    double h = (b - a) / n_children;
    double partial_integral, local_a, local_b;

    for (int i = 0; i < n_children; i++)
    {
        if (pipe(pipefd) == -1)
        {
            perror("pipe");
            return 1;
        }

        pid[i] = fork();

        if (pid[i] == -1)
        {
            perror("fork");
            return 1;
        }
        else if (pid[i] == 0)
        {
            /* Child process */
            close(pipefd[0]);

            local_a = a + i * h;
            local_b = local_a + h;

            partial_integral = integrate(local_a, local_b, width);

            write(pipefd[1], &partial_integral, sizeof(double));
            close(pipefd[1]);
            exit(0);
        }
        else
        {
            /* Parent process */
            close(pipefd[1]);
            waitpid(pid[i], &status, 0);
            read(pipefd[0], &partial_integral, sizeof(double));

            integral_value += partial_integral;
        }
    }

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Integral value: %.10f | Time: %.5f s\n", integral_value, cpu_time_used);

    return 0;
}
