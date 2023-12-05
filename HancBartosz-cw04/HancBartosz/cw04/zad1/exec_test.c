#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define SIGNAL SIGUSR1
int parent = 1;

enum OPTION
{
    IGNORE,
    MASK,
    PENDING,
    INVALID
};

int parse_args(const int argc, const char *argv)
{
    if (argc == 3)
    {
        parent = 0;
    }

    if (strcmp(argv, "ignore") == 0)
        return IGNORE;

    if (strcmp(argv, "mask") == 0)
        return MASK;

    if (strcmp(argv, "pending") == 0)
        return PENDING;

    return INVALID;
}

void sighandler()
{
    printf("Received signal %d\n", getpid());
}

void execute(int option)
{
    switch (option)
    {
    case IGNORE:
        signal(SIGNAL, SIG_IGN);
        raise(SIGNAL);
        printf("Raise signal %d\n", getpid());
        break;

    case MASK:
        signal(SIGNAL, sighandler);
        if (parent)
        {
            struct sigaction action;

            sigemptyset(&action.sa_mask);
            sigaddset(&action.sa_mask, SIGNAL);
            sigprocmask(SIG_BLOCK, &action.sa_mask, NULL);

            raise(SIGNAL);
            printf("Raise signal %d\n", getpid());
        }
        break;

    case PENDING:
        signal(SIGNAL, sighandler);
        if (parent)
        {
            struct sigaction action;

            sigemptyset(&action.sa_mask);
            sigaddset(&action.sa_mask, SIGNAL);
            sigprocmask(SIG_BLOCK, &action.sa_mask, NULL);

            raise(SIGNAL);
            printf("Raise signal %d\n", getpid());
        }

        sigset_t sigset;
        sigpending(&sigset);
        if (sigismember(&sigset, SIGNAL))
        {
            printf("Signal pending %d\n", getpid());
        }
        else
        {
            printf("Signal not pending %d\n", getpid());
        }

        break;

    case INVALID:
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3)
    {
        puts("Incorrect number of arguments!");
        return 0;
    }

    int option = parse_args(argc, argv[1]);
    if (option == INVALID)
    {
        puts("Invalid option");
        return 0;
    }

    execute(option);
    fflush(NULL);

    if (parent)
    {
        execl(argv[0], argv[0], argv[1], "child", NULL);
    }
    else
    {
        wait(NULL);
    }

    fflush(NULL);

    return 0;
}