#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>

#define HOME_PATH getenv("HOME")
#define MAX_CLIENTS 10
#define MSG_SIZE 100
#define SERVER_QKEY ftok(HOME_PATH, 0)
#define KEY_GEN_NUM 200

typedef struct msgbuf
{
    long mtype;
    key_t qkey;
    char msg[MSG_SIZE];
    int from_id;
    int to_id;
    struct tm tm;
} msgbuf;

typedef enum mtype
{
    INIT = 1,
    LIST = 2,
    TO_ONE = 3,
    TO_ALL = 4,
    STOP = 5
} mtype;
