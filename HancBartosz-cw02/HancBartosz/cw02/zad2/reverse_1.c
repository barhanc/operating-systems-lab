#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

// Wariant open(), lseek(), read(), write(), close()

int main(int argc, char *argv[])
{
    struct timespec timespec_start, timespec_end;
    clock_gettime(CLOCK_REALTIME, &timespec_start);

    if (argc != 1 + 2)
    {
        puts("Incorrect number of arguments!");
        return 0;
    }

    int infile = open(argv[1], O_RDONLY);
    int outfile = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (infile == -1 || outfile == -1)
    {
        puts("Error ocurred while opening files!");
        return 0;
    }

    char buffer;
    int len = lseek(infile, -1, SEEK_END);
    while (len > -1)
    {
        if (read(infile, &buffer, 1) == -1)
        {
            puts("Error ocurred while reading from input!");
            return 0;
        }
        len = lseek(infile, -2, SEEK_CUR);
        if (write(outfile, &buffer, 1) == -1)
        {
            puts("Error ocurred while writing to output!");
            return 0;
        }
    }

    clock_gettime(CLOCK_REALTIME, &timespec_end);
    printf("Real: %ld ns\n", timespec_end.tv_nsec - timespec_start.tv_nsec);

    return 0;
}