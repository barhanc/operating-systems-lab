#include "defs.h"

int chair_semids[N];
int barber_semids[M];

int shm_semid[1];
int shmid;

pid_t children[M];
pid_t helper;

void sighandler(int signo)
{

    if (helper == 0)
    {
        puts("Barbershop closing...");
        for (int i = 0; i < M; i++)
        {
            if (children[i] != -1)
            {
                kill(children[i], SIGKILL);
            }
        }

        close_sems(barber_semids, M);
        close_sems(chair_semids, N);
        close_sems(shm_semid, 1);
    }

    exit(0);
}

// Function simulating a haircut. I assume that time of haircut is its number (int) in seconds
void cut_hair(int barber_no, int chair_no, int haircut_no)
{
    printf("Barber No.%d WORKS    at chair No.%d (TIME %ds)\n", barber_no, chair_no, haircut_no);
    sleep(haircut_no);
    printf("Barber No.%d FINISHED at chair No.%d\n", barber_no, chair_no);

    unlock(chair_semids[chair_no]);
    unlock(barber_semids[barber_no]);
    exit(0);
}

int main()
{
    // Set random seed
    srand(time(NULL));

    // Set handling of SIGINT
    signal(SIGINT, sighandler);

    // Initialize structures
    create_sems(chair_semids, N);
    create_sems(barber_semids, M);

    create_sems(shm_semid, 1);
    shmid = create_shm();

    for (int i = 0; i < M; i++)
    {
        children[i] = -1;
    }
    helper = fork();

    if (helper == 0) // Child process manages finding free barber and chair for customers in queue
    {
        int *ptr_q = get_shm(shmid);

        while (true)
        {
            if (ptr_q[0] == -1)
            {
                continue;
            }

            bool found_barber = false;
            bool found_chair = false;
            int barber_no;
            int chair_no;

            while (!found_barber)
            {
                for (int i = 0; i < M; i++)
                {
                    if (getlock(barber_semids[i]) == 0)
                    {
                        lock(barber_semids[i]);
                        found_barber = true;
                        barber_no = i;
                        break;
                    }
                }
            }

            while (!found_chair)
            {
                for (int i = 0; i < N; i++)
                {
                    if (getlock(chair_semids[i]) == 0)
                    {
                        lock(chair_semids[i]);
                        found_chair = true;
                        chair_no = i;
                        break;
                    }
                }
            }

            while (getlock(shm_semid[0]) != 0)
                ;

            lock(shm_semid[0]);
            int haircut_no = pop(ptr_q);
            unlock(shm_semid[0]);
            if ((children[barber_no] = fork()) == 0)
            {
                cut_hair(barber_no, chair_no, haircut_no);
            }
        }
    }
    else // Parent process adds new customers to queue if it isn't full
    {
        lock(shm_semid[0]);
        int *ptr_q = get_shm(shmid);
        initialize(ptr_q);
        unlock(shm_semid[0]);

        puts("[Press ENTER to spawn new client]");
        while (true)
        {
            while (getchar() != '\n')
                ;
            while (getlock(shm_semid[0]) != 0)
                ;

            lock(shm_semid[0]);
            int haircut_no = 1 + (rand() % F);

            if (ptr_q[P - 1] == -1)
            {
                push(ptr_q, haircut_no);
                printf("New client with haircut No. %d\n", haircut_no);
            }
            else
            {
                puts("New client spawned but queue is full!");
            }

            unlock(shm_semid[0]);
        }
    }
}