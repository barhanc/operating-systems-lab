#include "libwc.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <regex.h>

#define LINE_BUFF_SIZE 1000

enum COMMAND
{
    INIT,
    COUNT,
    SHOW,
    DELETE,
    DESTROY,
    QUIT,
    HELP
};

typedef struct Command
{
    enum COMMAND COMMAND;
    union
    {
        int arg;
        char *file_name;
    };

} Command;

regex_t rx_init;
regex_t rx_count;
regex_t rx_show;
regex_t rx_delete;
regex_t rx_destroy;
regex_t rx_quit;
regex_t rx_help;

Command read()
{
    regcomp(&rx_init, "^init [0-9][0-9]*\n$", 0);
    regcomp(&rx_count, "^count ..*\n$", 0);
    regcomp(&rx_show, "^show [0-9][0-9]*\n$", 0);
    regcomp(&rx_delete, "^delete [0-9][0-9]*\n$", 0);
    regcomp(&rx_destroy, "^destroy\n$", 0);
    regcomp(&rx_quit, "^quit\n$", 0);
    regcomp(&rx_help, "^help\n$", 0);

    char input[LINE_BUFF_SIZE] = "";
    char command_name[LINE_BUFF_SIZE] = "";
    char file_name[LINE_BUFF_SIZE] = "";
    int arg = -1;

    char *err = fgets(input, sizeof input, stdin);
    if (err == NULL)
    {
        printf("Error ocurred while reading input!");
        return (Command){
            .COMMAND = HELP,
            .arg = -1,
        };
    }

    if (!regexec(&rx_init, input, 0, NULL, 0))
    {
        sscanf(input, "%s %d", command_name, &arg);
        return (Command){
            .COMMAND = INIT,
            .arg = arg,
        };
    }

    if (!regexec(&rx_count, input, 0, NULL, 0))
    {
        sscanf(input, "%s %s", command_name, file_name);
        return (Command){
            .COMMAND = COUNT,
            .file_name = file_name,
        };
    }

    if (!regexec(&rx_show, input, 0, NULL, 0))
    {
        sscanf(input, "%s %d", command_name, &arg);
        return (Command){
            .COMMAND = SHOW,
            .arg = arg,
        };
    }

    if (!regexec(&rx_delete, input, 0, NULL, 0))
    {
        sscanf(input, "%s %d", command_name, &arg);
        return (Command){
            .COMMAND = DELETE,
            .arg = arg,
        };
    }

    if (!regexec(&rx_destroy, input, 0, NULL, 0))
    {
        return (Command){
            .COMMAND = DESTROY,
            .arg = -1,
        };
    }

    if (!regexec(&rx_quit, input, 0, NULL, 0))
    {
        return (Command){
            .COMMAND = QUIT,
            .arg = -1,
        };
    }

    if (!regexec(&rx_help, input, 0, NULL, 0))
    {
        return (Command){
            .COMMAND = HELP,
            .arg = -1,
        };
    }

    regfree(&rx_init);
    regfree(&rx_count);
    regfree(&rx_show);
    regfree(&rx_delete);
    regfree(&rx_destroy);
    regfree(&rx_quit);
    regfree(&rx_help);

    puts("Invalid command!");

    return (Command){
        .COMMAND = HELP,
        .arg = -1,
    };
}

libwc_mem mem;
bool initialized = false;
bool running = true;

void eval(Command *command)
{
    switch (command->COMMAND)
    {
    case INIT:
        if (!initialized)
        {
            initialized = true;
            mem = libwc_create(command->arg);
            // printf("Initialized structure with size %d\n", command->arg);
        }
        else
        {
            puts("Structure already initialized!");
        }
        break;

    case COUNT:
        if (!initialized)
        {
            puts("Structure not initialized!");
        }
        else
        {
            libwc_wc(&mem, command->file_name);
        }
        break;

    case SHOW:
        if (!initialized)
        {
            puts("Structure not initialized!");
        }
        else
        {
            puts(libwc_get_at(&mem, command->arg));
        }
        break;

    case DELETE:
        if (!initialized)
        {
            puts("Structure not initialized!");
        }
        else
        {
            libwc_delete_at(&mem, command->arg);
        }
        break;

    case DESTROY:
        if (!initialized)
        {
            puts("Structure not initialized!");
        }
        else
        {
            libwc_destroy(&mem);
            initialized = false;
            // puts("Structure destroyed");
        }
        break;

    case QUIT:
        running = false;
        break;

    case HELP:
        puts("Available commands:\n\n>> init (int) n\n\n>> count (str) file_name\n\n>> show (int) i\n\n>> delete (int) i\n\n>> destroy\n\n>> quit\n\n>> help\n");
        break;
    }
}

int main()
{
    struct timespec timespec_start, timespec_end;
    struct tms tms_start, tms_end;

    while (running)
    {
        printf("libwc REPL >>> ");

        clock_gettime(CLOCK_REALTIME, &timespec_start);
        times(&tms_start);

        Command command = read();
        eval(&command);

        clock_gettime(CLOCK_REALTIME, &timespec_end);
        times(&tms_end);

        printf("Real: %ld ns\n", timespec_end.tv_nsec - timespec_start.tv_nsec);
        printf("User: %ld ticks\n", tms_end.tms_cutime - tms_start.tms_cutime);
        printf("Syst: %ld ticks\n\n", tms_end.tms_cstime - tms_start.tms_cstime);
        /*
        The number of cloc icks per second can be obtained using
        sysconf(_SC_CLK_TCK);
        */
    }

    return 0;
}
