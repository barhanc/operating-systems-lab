#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MAX_N_OPTIONS 1000

pid_t catcher_pid;
int options[MAX_N_OPTIONS];
int len;
int i = 0;

void send_option(int option)
{
    // sleep(10);
    printf(">>> Sending option: %d\n\n", option);
    union sigval val;
    val.sival_int = option;
    sigqueue(catcher_pid, SIGUSR1, val);
    i++;
}

void handler(int sig, siginfo_t *info, void *context)
{
    puts(">>> Received catcher signal.");
    if (i < len)
    {
        send_option(options[i]);
        puts(">>> Waiting for catcher signal...");
        pause();
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > MAX_N_OPTIONS)
    {
        puts("Incorrect number of arguments!");
    }

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;

    if (sigaction(SIGUSR1, &sa, NULL) != 0)
    {
        perror("sigaction");
        return 1;
    }

    catcher_pid = atoi(argv[1]);
    len = argc - 2;
    for (int i = 2; i < argc; i++)
    {
        options[i - 2] = atoi(argv[i]);
    }

    send_option(options[i]);
    puts(">>> Waiting for catcher signal...");
    pause();

    return 0;
}