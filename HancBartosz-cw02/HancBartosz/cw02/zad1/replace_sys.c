#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

// Wariant read() write()

int main(int argc, char *argv[])
{
    struct timespec timespec_start, timespec_end;
    clock_gettime(CLOCK_REALTIME, &timespec_start);

    char find_char[2] = "";
    char swap_char[2] = "";

    if (argc != 1 + 4)
    {
        puts("Incorrect number of arguments!\n");
        return 0;
    }
    if (strlen(argv[1]) != 1 || strlen(argv[2]) != 1)
    {
        printf("Incorrect argument!\n");
        return 0;
    }
    strcat(find_char, argv[1]);
    strcat(swap_char, argv[2]);

    int infile = open(argv[3], O_RDONLY);
    int outfile = open(argv[4], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (infile == -1 || outfile == -1)
    {
        puts("Error ocurred while opening files!");
        return 0;
    }

    char c;
    while (read(infile, &c, 1) == 1)
    {
        if (c == find_char[0])
        {
            c = swap_char[0];
        }
        if (write(outfile, &c, 1) == -1)
        {
            puts("Error ocurred while writing to output!");
            return 0;
        }
    }

    clock_gettime(CLOCK_REALTIME, &timespec_end);

    printf("Real: %ld ns\n", timespec_end.tv_nsec - timespec_start.tv_nsec);

    return 0;
}