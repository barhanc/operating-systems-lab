#include "defs.h"

#define MAX_CLIENTS 10

typedef struct
{
    int fd;
    int id;
    char name[MAX_MESSAGE_LEN];
    time_t last_ping;
} Client;

Client clients[MAX_CLIENTS];

int port = 7777;
int running = 1;
int epoll_fd;

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

int create_server_socket_tcp();
void handle_new_client(int client_fd);
void handle_client_message(int client_fd, char *message);
void *ping();
void sighandler(int signo)
{
    close(epoll_fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    memset(clients, -1, sizeof(clients));
    signal(SIGINT, sighandler);

    int server_fd_tcp = create_server_socket_tcp();
    if (server_fd_tcp == -1)
    {
        printf("Error creating tcp socket\n");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create");
        close(server_fd_tcp);
        return 1;
    }

    struct epoll_event event;
    event.data.fd = server_fd_tcp;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd_tcp, &event) == -1)
    {
        perror("epoll_ctl");
        return 1;
    }
    struct epoll_event *events = malloc(MAX_EVENTS * sizeof(struct epoll_event));

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    while (running)
    {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++)
        {
            if (events[i].data.fd == server_fd_tcp)
            {
                int client_fd = accept(events[i].data.fd, NULL, NULL);
                if (client_fd == -1)
                {
                    printf("Error accepting new client\n");
                    continue;
                }
                struct epoll_event event;
                event.events = EPOLLIN;
                event.data.fd = client_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
                {
                    perror("epoll_ctl");
                    exit(1);
                }
                handle_new_client(client_fd);
            }
            else
            {
                int client_fd = events[i].data.fd;
                char buffer[MAX_MESSAGE_LEN];
                ssize_t num_bytes = read(client_fd, buffer, sizeof(buffer));
                buffer[num_bytes] = '\0';
                handle_client_message(client_fd, buffer);
            }
        }
    }

    return 0;
}

int create_server_socket_tcp()
{
    int fd = -1;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error creating socket\n");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_zero[0] = '\0';

    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Error binding\n");
        return -1;
    }

    if (listen(fd, 10) == -1)
    {
        printf("Error listening\n");
        return -1;
    }

    return fd;
}

void handle_new_client(int client_fd)
{
    char buffer[MAX_MESSAGE_LEN] = {0};

    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == -1)
        {
            clients[i].last_ping = time(NULL);
            clients[i].fd = client_fd;
            clients[i].id = i;
            read(client_fd, buffer, 64);
            sprintf(clients[i].name, buffer, i);
            break;
        }
    }

    if (i == MAX_CLIENTS)
    {
        sprintf(buffer, "Server is full. Can't accept new client.\n");
        write(client_fd, buffer, strlen(buffer));
        close(client_fd);
        return;
    }

    sprintf(buffer, "Welcome, %s!\n", clients[i].name);
    write(client_fd, buffer, strlen(buffer));
    printf("New client connected: '%s'(%d)\n", clients[i].name, clients[i].id);
}

void handle_client_message(int client_fd, char *message)
{
    compile_regex();
    char buffer[MAX_MESSAGE_LEN] = {0};
    int sender_id = -1;
    char sender_name[MAX_MESSAGE_LEN] = {0};

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == client_fd)
        {
            sender_id = clients[i].id;
            strcpy(sender_name, clients[i].name);
            clients[i].last_ping = time(NULL);
            break;
        }
    }

    if (!regexec(&rx_list, message, 0, NULL, 0))
    {
        printf("Received command 'LIST' from (%d)\n", sender_id);

        char line[MAX_MESSAGE_LEN];
        sprintf(line, "\nActive users:\n");
        strcpy(buffer, line);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].fd != -1)
            {

                sprintf(line, "(%d) %s\n", clients[i].id, clients[i].name);
                strcat(buffer, line);
            }
        }
        write(client_fd, buffer, strlen(buffer));
    }
    else if (!regexec(&rx_2all, message, 0, NULL, 0))
    {
        printf("Received command '2ALL' from (%d)\n", sender_id);
        sprintf(buffer, "Message from %s:\n%s", sender_name, message);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].fd != -1 && clients[i].fd != client_fd)
            {
                write(clients[i].fd, buffer, strlen(buffer));
            }
        }
    }

    else if (!regexec(&rx_2one, message, 0, NULL, 0))
    {
        printf("Received command '2ONE' from (%d)\n", sender_id);

        char command[MAX_MESSAGE_LEN] = {0};
        char text[MAX_MESSAGE_LEN] = {0};
        int to_i;
        sscanf(message, "%s %d %s", command, &to_i, text);
        if (to_i < MAX_CLIENTS && clients[to_i].fd != -1)
        {
            sprintf(buffer, "Message from %s:\n%s", sender_name, message);
            write(clients[to_i].fd, buffer, strlen(buffer));
        }
        else
        {
            char comm[64] = "No such user!";
            write(client_fd, comm, strlen(comm));
        }
    }

    else if (!regexec(&rx_stop, message, 0, NULL, 0))
    {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = client_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, &event) == -1)
        {
            perror("epoll_ctl");
            exit(1);
        }
        close(client_fd);
    }
}

void *ping()
{
    while (running)
    {
        time_t current_time = time(NULL);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].fd != -1 && current_time - clients[i].last_ping > PING_INTERVAL)
            {
                printf("Client '%s'(%d) is not responding. Disconnecting...\n",
                       clients[i].name, clients[i].id);
                close(clients[i].fd);
                clients[i].fd = -1;
            }
        }
        sleep(PING_INTERVAL);
    }

    return NULL;
}
