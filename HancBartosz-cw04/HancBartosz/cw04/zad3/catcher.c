#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>

int received_sigs = 0;
time_t rawtime;
struct tm *timeinfo;

void execute(int option)
{
    switch (option)
    {
    case 1:
        for (int i = 1; i <= 100; i++)
        {
            printf("%d\n", i);
        }
        puts("");
        break;

    case 2:
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf(">>> Current local time and date: %s\n", asctime(timeinfo));
        break;

    case 3:
        printf(">>> Received signals: %d\n\n", received_sigs);
        break;

    case 4:
        while (1)
        {
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            printf(">>> Current local time and date: %s\n", asctime(timeinfo));
            sleep(1);
        }

    case 5:
        puts(">>> Exiting...");
        exit(0);
        break;
    }
}

void handler(int sig, siginfo_t *info, void *context)
{
    received_sigs++;
    pid_t sender_pid = info->si_pid;
    int option = (info->si_value).sival_int;

    printf(">>> Received sender signal with option: %d\n", option);

    puts(">>> Sending confirmation to sender...");
    kill(sender_pid, SIGUSR1);

    puts(">>> Executing option...");
    execute(option);

    puts(">>> Waiting for sender signal...");
    pause();
}

int main(int argc, char *argv[])
{
    puts(">>> Starting...");
    printf(">>> PID: %d\n\n", getpid());

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;

    if (sigaction(SIGUSR1, &sa, NULL) != 0)
    {
        perror("sigaction");
        return 1;
    }

    puts(">>> Waiting for sender signal...");
    pause();

    return 0;
}