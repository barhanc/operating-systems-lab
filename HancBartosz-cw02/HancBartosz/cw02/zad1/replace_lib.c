#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

// Wariant fread() fwrite()

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

    char *buffer = 0;
    long length;
    FILE *input = fopen(argv[3], "r");
    if (input)
    {
        fseek(input, 0, SEEK_END);
        length = ftell(input);
        fseek(input, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            if (fread(buffer, sizeof(char), length, input) == -1)
            {
                puts("Error ocurred while reading from input!");
                return 0;
            }
        }
        fclose(input);
        buffer[length] = '\0';
    }
    else
    {
        printf("Error ocurred while opening file %s!\n", argv[3]);
        return 0;
    }

    char command[1000] = "touch ";
    strcat(command, argv[4]);

    if (system(command) == -1)
    {
        printf("Error ocurred while creating file %s!\n", argv[4]);
        return 0;
    }

    for (int i = 0; i < length; i++)
    {
        if (buffer[i] == (char)find_char[0])
        {
            buffer[i] = (char)swap_char[0];
        }
    }

    FILE *output = fopen(argv[4], "w");
    if (output)
    {
        if (fwrite(buffer, sizeof(char), length, output) == -1)
        {
            puts("Error ocurred while writing to output!");
            return 0;
        }
        fclose(output);
    }
    else
    {
        printf("Error ocurred while opening file %s!\n", argv[4]);
        return 0;
    }

    free(buffer);

    clock_gettime(CLOCK_REALTIME, &timespec_end);
    printf("Real: %ld ns\n", timespec_end.tv_nsec - timespec_start.tv_nsec);

    return 0;
}