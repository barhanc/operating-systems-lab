#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/time.h>
#include <regex.h>
#include <string.h>
#include <signal.h>

#define MAX_MESSAGE_LEN 256
#define PING_INTERVAL 1
#define MAX_EVENTS 100