#include "defs.h"

int client_qkey[MAX_CLIENTS];
int server_qid;

void handle_init(msgbuf *p_msgbuf)
{
    int new_client_qkey = p_msgbuf->qkey;
    int new_client_id = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_qkey[i] == -1)
        {
            new_client_id = i;
            break;
        }
    }

    if (new_client_id != -1)
    {
        client_qkey[new_client_id] = new_client_qkey;
        printf("New client connected with id: %d\n", new_client_id);
    }

    p_msgbuf->from_id = new_client_id;
    int new_client_qid = msgget(new_client_qkey, 0);
    msgsnd(new_client_qid, p_msgbuf, sizeof(msgbuf), 0);
}

void handle_list(msgbuf *p_msgbuf)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_qkey[i] != -1)
        {
            char msg[MSG_SIZE] = "";
            snprintf(msg, MSG_SIZE, "- id %d is active\n", i);
            strcat(p_msgbuf->msg, msg);
        }
    }
    time_t t = time(NULL);
    p_msgbuf->tm = *localtime(&t);

    int client_qid = msgget(client_qkey[p_msgbuf->from_id], 0);
    msgsnd(client_qid, p_msgbuf, sizeof(msgbuf), 0);
}

void handle_2one(msgbuf *p_msgbuf)
{
    time_t t = time(NULL);
    p_msgbuf->tm = *localtime(&t);
    int client_qid;

    if (p_msgbuf->to_id < MAX_CLIENTS && client_qkey[p_msgbuf->to_id] != -1)
    {
        client_qid = msgget(client_qkey[p_msgbuf->to_id], 0);
        msgsnd(client_qid, p_msgbuf, sizeof(msgbuf), 0);
    }
    else
    {
        strcpy(p_msgbuf->msg, "Server was unable to send message!");
        client_qid = msgget(client_qkey[p_msgbuf->from_id], 0);
        msgsnd(client_qid, p_msgbuf, sizeof(msgbuf), 0);
    }
}

void handle_2all(msgbuf *p_msgbuf)
{
    time_t t = time(NULL);
    p_msgbuf->tm = *localtime(&t);
    int client_qid;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_qkey[i] != -1 && i != p_msgbuf->from_id)
        {
            client_qid = msgget(client_qkey[i], 0);
            msgsnd(client_qid, p_msgbuf, sizeof(msgbuf), 0);
        }
    }
}

void handle_stop(msgbuf *p_msgbuf)
{
    if (p_msgbuf->from_id != -1)
    {
        client_qkey[p_msgbuf->from_id] = -1;
        printf("Client %d disconected\n", p_msgbuf->from_id);
    }
}

void save_log(msgbuf *p_msgbuf)
{
    FILE *f = fopen("logs.txt", "a");
    time_t t = time(NULL);
    p_msgbuf->tm = *localtime(&t);

    fprintf(f, "Received at %d-%02d-%02d %02d:%02d:%02d ", p_msgbuf->tm.tm_year + 1900, p_msgbuf->tm.tm_mon + 1, p_msgbuf->tm.tm_mday, p_msgbuf->tm.tm_hour, p_msgbuf->tm.tm_min, p_msgbuf->tm.tm_sec);

    switch (p_msgbuf->mtype)
    {
    case INIT:
        fprintf(f, "from new client: INIT");
        break;

    case LIST:
        fprintf(f, "from %d: LIST", p_msgbuf->from_id);
        break;

    case TO_ONE:
        fprintf(f, "from %d to %d: 2ONE, msg: %s", p_msgbuf->from_id, p_msgbuf->to_id, p_msgbuf->msg);
        break;

    case TO_ALL:
        fprintf(f, "from %d: 2ALL, msg: %s", p_msgbuf->from_id, p_msgbuf->msg);
        break;

    case STOP:
        fprintf(f, "from %d: STOP", p_msgbuf->from_id);
        break;
    }

    fprintf(f, "\n");
    fclose(f);
}

void sighandler(int signo)
{
    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_qkey[i] != -1)
        {
            p_msgbuf->mtype = STOP;
            int client_qid = msgget(client_qkey[i], 0);
            msgsnd(client_qid, p_msgbuf, sizeof(msgbuf), 0);
            msgrcv(server_qid, p_msgbuf, sizeof(msgbuf), STOP, 0);
        }
    }

    save_log(p_msgbuf);
    puts("Server exiting...");
    msgctl(server_qid, IPC_RMID, NULL);
    exit(0);
}

int main()
{
    puts("Server starting...");
    server_qid = msgget(SERVER_QKEY, IPC_CREAT | 0666);

    if (server_qid == -1)
    {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_qkey[i] = -1;
    }

    signal(SIGINT, sighandler);

    msgbuf *p_msgbuf = malloc(sizeof(msgbuf));

    while (true)
    {
        msgrcv(server_qid, p_msgbuf, sizeof(msgbuf), -6, 0);
        save_log(p_msgbuf);

        switch (p_msgbuf->mtype)
        {
        case INIT:
            handle_init(p_msgbuf);
            break;

        case LIST:
            handle_list(p_msgbuf);
            break;

        case TO_ONE:
            handle_2one(p_msgbuf);
            break;

        case TO_ALL:
            handle_2all(p_msgbuf);
            break;

        case STOP:
            handle_stop(p_msgbuf);
            break;
        }
    }
}