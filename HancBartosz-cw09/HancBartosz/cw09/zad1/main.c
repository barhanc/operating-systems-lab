#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define ELVES 10
#define REINDEERS 9
#define ELVES_INQUEUE 3
#define MAX_LOOPS 3

int loop = 0;

pthread_t santa_thread;
pthread_t elf_thread[ELVES];
pthread_t reindeer_thread[REINDEERS];

int elf_id[ELVES];
int reindeer_id[REINDEERS];

int reindeers_q[REINDEERS] = {0};
int elves_q[ELVES_INQUEUE] = {0};

int reindeer_qlen = 0;
int elves_qlen = 0;

pthread_barrier_t elves_barrier;
pthread_barrier_t reindeers_barrier;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *santa_routine(void *arg)
{
    while (true)
    {
        pthread_mutex_lock(&mutex);

        while (reindeer_qlen < REINDEERS && elves_qlen < ELVES_INQUEUE)
        {
            pthread_cond_wait(&cond, &mutex);
        }

        printf("[MIKOŁAJ] Budzę się.\n");

        if (reindeer_qlen == REINDEERS)
        {
            printf("[MIKOŁAJ] Dostarczam prezenty.\n");
            sleep(2 + (rand() % 3));
            reindeer_qlen = 0;
            loop++;
            pthread_barrier_wait(&reindeers_barrier);
        }
        else if (elves_qlen == ELVES_INQUEUE)
        {
            printf("[MIKOŁAJ] Rozwiązuję problemy elfów ");
            for (int i = 0; i < elves_qlen; i++)
            {
                printf("%d ", elves_q[i]);
            }
            printf("\n");

            sleep(1 + (rand() % 2));
            elves_qlen = 0;
            pthread_barrier_wait(&elves_barrier);
        }

        printf("[MIKOŁAJ] Zasypiam.\n");

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *elf_routine(void *arg)
{
    int id = *(int *)arg;

    while (true)
    {
        sleep(2 + (rand() % 4));
        pthread_mutex_lock(&mutex);
        if (elves_qlen < ELVES_INQUEUE)
        {
            elves_q[elves_qlen] = id;
            elves_qlen++;
            if (elves_qlen == ELVES_INQUEUE)
            {
                printf("[ELF] Czeka %d elfów. Wybudzam Mikołaja, %d\n", elves_qlen, id);
            }
            else
            {
                printf("[ELF] Czeka %d elfów, %d\n", elves_qlen, id);
            }
            pthread_cond_signal(&cond);
        }
        else
        {
            printf("[ELF] Samodzielnie rozwiązuję swój problem, %d\n", id);
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pthread_mutex_unlock(&mutex);
        pthread_barrier_wait(&elves_barrier);
    }

    return NULL;
}

void *reindeer_routine(void *arg)
{
    int id = *(int *)arg;

    while (true)
    {
        printf("[RENIFER] Jestem na wakacjach, %d\n", id);
        sleep(5 + (rand() % 6));

        pthread_mutex_lock(&mutex);
        reindeers_q[reindeer_qlen] = id;
        reindeer_qlen++;

        if (reindeer_qlen == REINDEERS)
        {
            printf("[RENIFER] Czeka %d reniferów. Wybudzam Mikołaja, %d\n", reindeer_qlen, id);
        }
        else
        {
            printf("[RENIFER] Czeka %d reniferów, %d\n", reindeer_qlen, id);
        }

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        pthread_barrier_wait(&reindeers_barrier);
    }

    return NULL;
}

int main()
{
    pthread_barrier_init(&reindeers_barrier, NULL, REINDEERS + 1);
    pthread_barrier_init(&elves_barrier, NULL, ELVES_INQUEUE + 1);

    // Create reindeer threads
    for (int i = 0; i < REINDEERS; i++)
    {
        reindeer_id[i] = i;
        pthread_create(&reindeer_thread[i], NULL, reindeer_routine, (void *)(&reindeer_id[i]));
    }
    // Create elf threads
    for (int i = 0; i < ELVES; i++)
    {
        elf_id[i] = i;
        pthread_create(&elf_thread[i], NULL, elf_routine, (void *)(&elf_id[i]));
    }
    // Create Santa thread
    pthread_create(&santa_thread, NULL, santa_routine, NULL);

    while (loop < MAX_LOOPS)
        ;

    return 0;
}