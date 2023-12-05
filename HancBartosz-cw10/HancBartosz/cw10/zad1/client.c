#include "defs.h"

int running = 1;
int fd = -1;
int port = 7777;
char *name[64];

regex_t rx_stop;
regex_t rx_2one;
regex_t rx_2all;
regex_t rx_list;

void compile_regex()
{
    regcomp(&rx_stop, "^STOP\n$", 0);
    regcomp(&rx_2one, "^2ONE [0-9][0-9]* .*\n$", 0);
    regcomp(&rx_2all, "^2ALL .*\n$", 0);
    regcomp(&rx_list, "^LIST\n$", 0);
}

pthread_t ping_thread;
pthread_t send_thread;
pthread_t recv_thread;

void *ping();
void *send_msg();
void *receive_msg();

void handler(int signo)
{
    running = 0;
    pthread_cancel(ping_thread);
    pthread_cancel(send_thread);
    pthread_cancel(recv_thread);
    write(fd, "STOP\n", 6);

    close(fd);
    printf("Exiting...\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./client <name>\n");
        return 1;
    }
    strcpy(name, argv[1]);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_zero[0] = '\0';

    if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Error connecting\n");
        exit(EXIT_FAILURE);
    }

    if (write(fd, name, 64) == -1)
    {
        printf("Error sending message\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&ping_thread, NULL, ping, NULL);
    pthread_create(&send_thread, NULL, send_msg, NULL);
    pthread_create(&recv_thread, NULL, receive_msg, NULL);

    signal(SIGINT, handler);

    while (running)
        ;

    return 0;
}

void *ping()
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (running)
    {
        write(fd, "", 1);
        sleep(PING_INTERVAL);
    }

    return NULL;
}

void *send_msg()
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    compile_regex();

    while (running)
    {
        char buffer[MAX_MESSAGE_LEN] = {0};
        fgets(buffer, MAX_MESSAGE_LEN, stdin);
        if (!regexec(&rx_stop, buffer, 0, NULL, 0))
        {
            write(fd, buffer, strlen(buffer));
            raise(SIGINT);
        }
        else if (!regexec(&rx_list, buffer, 0, NULL, 0) || !regexec(&rx_2all, buffer, 0, NULL, 0) || !regexec(&rx_2one, buffer, 0, NULL, 0))
        {
            write(fd, buffer, strlen(buffer));
        }
        else
        {
            printf("Usage: STOP | LIST | 2ALL <message> | 2ONE <client_id> <message>\n");
        }
    }

    return NULL;
}

void *receive_msg()
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (running)
    {
        char buffer[MAX_MESSAGE_LEN] = {0};
        ssize_t num_bytes = read(fd, buffer, sizeof(buffer));
        if (num_bytes > 0)
        {
            printf("%s\n", buffer);
        }
    }

    return NULL;
}
