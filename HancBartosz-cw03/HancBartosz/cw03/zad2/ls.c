#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("%s ", argv[0]);

    if (argc != 1 + 1)
    {
        puts("Incorrect number of arguments!");
        return 0;
    }
    fflush(stdout);
    execl("/bin/ls", "ls", argv[1], NULL);

    return 0;
}