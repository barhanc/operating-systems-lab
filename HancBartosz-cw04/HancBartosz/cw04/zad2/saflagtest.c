#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MAX_CNT 10

void handler1(int sig, siginfo_t *info, void *context)
{
    printf("Received signal %d\n", sig);
    printf("Sending process ID: %d\n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    printf("User time: %lu\n", (unsigned long)info->si_utime);
    printf("System time: %lu\n", (unsigned long)info->si_stime);
}

int cnt = 0;

void handler2(int sig)
{
    cnt++;
    printf("Signal %d received. (%d)\n", sig, cnt);
    if (cnt < MAX_CNT)
    {
        raise(SIGINT);
    }
}

void handler3(int sig)
{
    printf("Signal %d received.\n", sig);
    printf("Resetting signal handler...\n");
    printf("Waiting for signal...\n");
    raise(SIGUSR1);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        puts("Incorrect number of arguments!");
        return 0;
    }

    if (strcmp(argv[1], "SA_SIGINFO") == 0)
    {
        struct sigaction sa;
        sa.sa_sigaction = handler1;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_SIGINFO;

        if (sigaction(SIGINT, &sa, NULL) != 0)
        {
            perror("sigaction");
            return 1;
        }

        printf("Waiting for SIGINT signal...\n");
        raise(SIGINT);
        printf("Exiting...\n");

        return 0;
    }

    if (strcmp(argv[1], "SA_NODEFER") == 0)
    {
        struct sigaction sa;
        sa.sa_handler = handler2;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_NODEFER;

        if (sigaction(SIGINT, &sa, NULL) != 0)
        {
            perror("sigaction");
            return 1;
        }

        printf("Waiting for SIGINT signal...\n");
        printf("Max received signals set to: %d\n", MAX_CNT);
        raise(SIGINT);
        printf("Exiting...\n");

        return 0;
    }

    if (strcmp(argv[1], "SA_RESETHAND") == 0)
    {
        struct sigaction sa;
        sa.sa_handler = handler3;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESETHAND;

        if (sigaction(SIGUSR1, &sa, NULL) != 0)
        {
            perror("sigaction");
            return 1;
        }

        printf("Waiting for signal...\n");
        raise(SIGUSR1);
        printf("Exiting...\n");

        return 0;
    }

    puts("Invalid argument! Available options: SA_SIGINFO, SA_NODEFER, SA_RESETHAND");
    return 0;
}
