#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAIL_LENGTH 100
#define MAX_SUBJECT_LENGTH 100
#define MAX_BODY_LENGTH 1000

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        char command[MAX_EMAIL_LENGTH + 20];
        char *sort_option = strcmp(argv[1], "data") == 0 ? "-k 2" : "-k 3";

        sprintf(command, "mail -H | sort %s", sort_option);

        FILE *fp = popen(command, "r");
        if (fp == NULL)
        {
            return 1;
        }

        printf("Lista e-maili:\n");
        char line[MAX_EMAIL_LENGTH + 20];

        while (fgets(line, sizeof(line), fp) != NULL)
        {
            printf("%s", line);
        }
        pclose(fp);
    }

    else if (argc == 4)
    {
        char command[MAX_EMAIL_LENGTH + MAX_SUBJECT_LENGTH + MAX_BODY_LENGTH + 50];
        sprintf(command, "echo \"%s\" | mail -s \"%s\" %s", argv[3], argv[2], argv[1]);
        if (system(command) == 1)
        {
            printf("E-mail wysłany\n");
        }
    }

    else
    {
        printf("Incorrect number arguments!\n");
        return 1;
    }

    return 0;
}
