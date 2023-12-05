#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

// Wariant open(), lseek(), read(), write(), close()

void reverse(char *buffer, char *dest_buffer)
{
    int len = strlen(buffer);
    for (int i = 0; i < len; i++)
    {
        dest_buffer[len - i - 1] = buffer[i];
    }
}

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

    char buffer[1024];
    char *rev_buffer;

    int len = lseek(infile, -sizeof(buffer), SEEK_END);
    while (len > -1)
    {
        if (read(infile, buffer, sizeof(buffer)) == -1)
        {
            puts("Error ocurred while reading from input!");
            return 0;
        }
        len = lseek(infile, -2 * sizeof(buffer), SEEK_CUR);

        rev_buffer = calloc(sizeof(buffer), sizeof(char));
        reverse(buffer, rev_buffer);
        if (write(outfile, rev_buffer, sizeof(buffer)) == -1)
        {
            puts("Error ocurred while writing to output!");
            return 0;
        }
        free(rev_buffer);
    }

    len = lseek(infile, 0, SEEK_END);
    len = len % sizeof(buffer);
    char new_buffer[len + 1];

    lseek(infile, 0, SEEK_SET);
    if (read(infile, new_buffer, sizeof(new_buffer)) == -1)
    {
        puts("Error ocurred while reading from input!");
        return 0;
    }

    new_buffer[len] = '\0';

    rev_buffer = calloc(sizeof(new_buffer), sizeof(char));
    reverse(new_buffer, rev_buffer);

    if (write(outfile, rev_buffer, len) == -1)
    {
        puts("Error ocurred while writing to output!");
        return 0;
    }
    free(rev_buffer);

    clock_gettime(CLOCK_REALTIME, &timespec_end);
    printf("Real: %ld ns\n", timespec_end.tv_nsec - timespec_start.tv_nsec);

    return 0;
}