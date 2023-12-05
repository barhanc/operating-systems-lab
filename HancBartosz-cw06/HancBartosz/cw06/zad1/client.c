#include "defs.h"

#define COMMAND_SIZE 200

int my_id = -1;
int my_qkey;
int my_qid;
int server_qid;

regex_t rx_stop;
regex_t rx_2one;
regex_t rx_2all;
regex_t rx_list;
pid_t child;

void handle_init()
{
    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
    p_msgbuf->mtype = INIT;
    p_msgbuf->qkey = my_qkey;

    int err = msgsnd(server_qid, p_msgbuf, sizeof(msgbuf), 0);
    if (err == -1)
    {
        printf("%s\n", strerror(errno));
    }

    msgrcv(my_qid, p_msgbuf, sizeof(msgbuf), INIT, 0);
    if (p_msgbuf->from_id == -1)
    {
        msgctl(my_qid, IPC_RMID, NULL);
        puts("Server is full! Exiting...");
        exit(0);
    }

    my_id = p_msgbuf->from_id;
    printf("Connected to server. Id: %d\n", my_id);
}

void handle_list()
{
    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
    p_msgbuf->from_id = my_id;
    p_msgbuf->mtype = LIST;
    p_msgbuf->qkey = my_qkey;

    msgsnd(server_qid, p_msgbuf, sizeof(msgbuf), 0);
}

void handle_2all(char *msg)
{
    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
    p_msgbuf->from_id = my_id;
    p_msgbuf->mtype = TO_ALL;
    p_msgbuf->qkey = my_qkey;
    strcpy(p_msgbuf->msg, msg);

    msgsnd(server_qid, p_msgbuf, sizeof(msgbuf), 0);
}

void handle_2one(int to_id, char *msg)
{
    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
    p_msgbuf->from_id = my_id;
    p_msgbuf->mtype = TO_ONE;
    p_msgbuf->qkey = my_qkey;
    p_msgbuf->to_id = to_id;
    strcpy(p_msgbuf->msg, msg);

    msgsnd(server_qid, p_msgbuf, sizeof(msgbuf), 0);
}

void sighandler(int signo)
{
    if (child != 0)
    {
        msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
        p_msgbuf->from_id = my_id;
        p_msgbuf->mtype = STOP;
        p_msgbuf->qkey = my_qkey;
        msgsnd(server_qid, p_msgbuf, sizeof(msgbuf), 0);
        msgctl(my_qid, IPC_RMID, NULL);

        puts("Exiting...");
        kill(child, SIGTERM);
        waitpid(child, NULL, 0);
        exit(0);
    }
}

int main()
{
    srand(time(NULL));
    my_qkey = ftok(HOME_PATH, 1 + rand() % KEY_GEN_NUM);

    my_qid = msgget(my_qkey, IPC_CREAT | 0666);
    if (my_qid == -1)
    {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    server_qid = msgget(SERVER_QKEY, 0);
    if (server_qid == -1)
    {
        msgctl(my_qid, IPC_RMID, NULL);
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    signal(SIGINT, sighandler);

    handle_init();

    regcomp(&rx_stop, "^STOP\n$", 0);
    regcomp(&rx_2one, "^2ONE [0-9][0-9]* .*\n$", 0);
    regcomp(&rx_2all, "^2ALL .*\n$", 0);
    regcomp(&rx_list, "^LIST\n$", 0);

    child = fork();
    if (child == 0) // Child process
    {
        char input[COMMAND_SIZE] = "";
        char command[COMMAND_SIZE] = "";
        int to_id = -1;
        char msg[MSG_SIZE] = "";

        while (true)
        {
            printf(">>> ");
            fgets(input, COMMAND_SIZE, stdin);

            if (!regexec(&rx_stop, input, 0, NULL, 0))
            {
                kill(getppid(), SIGINT);
            }

            else if (!regexec(&rx_list, input, 0, NULL, 0))
            {
                handle_list();
            }

            else if (!regexec(&rx_2all, input, 0, NULL, 0))
            {
                sscanf(input, "%s %s", command, msg);
                handle_2all(msg);
            }

            else if (!regexec(&rx_2one, input, 0, NULL, 0))
            {
                sscanf(input, "%s %d %s", command, &to_id, msg);
                handle_2one(to_id, msg);
            }

            else if (strcmp(input, "\n") != 0)
            {
                puts("Invalid comand! Available commands: STOP, LIST, 2ALL [MSG], 2ONE [TO_ID] [MSG]");
            }
        }
    }
    else // Parent process
    {
        while (true)
        {
            msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
            msgrcv(my_qid, p_msgbuf, sizeof(msgbuf), 0, 0);

            switch (p_msgbuf->mtype)
            {
            case STOP:
                puts("Server is exiting...");
                raise(SIGINT);
                break;

            case TO_ALL:
            case TO_ONE:
                printf("Received message from %d sent at %d-%02d-%02d %02d:%02d:%02d\n%s\n", p_msgbuf->from_id, p_msgbuf->tm.tm_year + 1900, p_msgbuf->tm.tm_mon + 1, p_msgbuf->tm.tm_mday, p_msgbuf->tm.tm_hour, p_msgbuf->tm.tm_min, p_msgbuf->tm.tm_sec, p_msgbuf->msg);
                break;

            case LIST:
                printf("Active clients at %d-%02d-%02d %02d:%02d:%02d\n%s\n", p_msgbuf->tm.tm_year + 1900, p_msgbuf->tm.tm_mon + 1, p_msgbuf->tm.tm_mday, p_msgbuf->tm.tm_hour, p_msgbuf->tm.tm_min, p_msgbuf->tm.tm_sec, p_msgbuf->msg);
                break;
            }
        }
    }
}