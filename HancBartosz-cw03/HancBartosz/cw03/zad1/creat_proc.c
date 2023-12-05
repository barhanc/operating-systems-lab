#include <stdio.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 1 + 1)
    {
        puts("Incorrect number of arguments");
        return 0;
    }

    int n;
    if (sscanf(argv[1], "%d", &n) != 1)
    {
        puts("Error ocurred while reading from input!");
        return 0;
    };

    pid_t child_pid;
    for (int i = 0; i < n; i++)
    {
        child_pid = fork();
        if (child_pid == 0)
        {
            printf("Proces rodzica ma PID: %d | Proces dziecka ma PID: %d\n", getppid(), getpid());
            exit(0);
        }
    }
    for (int i = 0; i < n; i++)
    {
        wait(NULL);
    }

    printf("%d\n", n);
    return 0;
}