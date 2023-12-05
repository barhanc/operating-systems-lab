#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 3 // no chairs
#define M 5 // no barbers (assuming M >= N)
#define P 5 // queue len
#define F 7 // no haircuts

#define HOME_PATH getenv("HOME")
#define KEY_GEN_NUM 1000

// P element FIFO queue

void push(int queue[], int el)
{
    int j = -1;
    for (int i = 0; i < P; i++)
    {
        if (queue[i] == -1)
        {
            j = i;
            break;
        }
    }
    if (j == -1)
    {
        puts("Queue is full!");
        return;
    }

    queue[j] = el;
}

int pop(int queue[])
{
    if (queue[0] == -1)
    {
        puts("Queue is empty!");
        return -1;
    }

    int el = queue[0];
    int i = 0;

    while (queue[i] != -1 && i + 1 < P)
    {
        queue[i] = queue[i + 1];
        i++;
    }

    for (int j = i; j < P; j++)
    {
        queue[j] = -1;
    }

    return el;
}

void initialize(int queue[])
{
    for (int i = 0; i < P; i++)
    {
        queue[i] = -1;
    }
}

void create_sems(int sems[], int n)
{
    for (int i = 0; i < n; i++)
    {
        key_t key = ftok(HOME_PATH, rand() % KEY_GEN_NUM);
        int semid = semget(key, 1, IPC_CREAT | 0666);
        sems[i] = semid;
        if (semid == -1)
        {
            printf("Error: %s\n", strerror(errno));
            return;
        }

        if (semctl(semid, 0, SETVAL, 0) == -1)
        {
            printf("Error: %s\n", strerror(errno));
            return;
        }
    }
}

void close_sems(int sems[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (semctl(sems[i], 0, IPC_RMID) == -1)
        {
            printf("Error: %s\n", strerror(errno));
            return;
        }
    }
}

void lock(int semid)
{
    semctl(semid, 0, SETVAL, 1);
}

void unlock(int semid)
{
    semctl(semid, 0, SETVAL, 0);
}

int getlock(int semid)
{
    return semctl(semid, 0, GETVAL);
}

int create_shm()
{
    return shmget(ftok(HOME_PATH, rand() % KEY_GEN_NUM), P * sizeof(int), IPC_CREAT | 0666);
}

void *get_shm(int shmid)
{
    return shmat(shmid, NULL, 0);
}