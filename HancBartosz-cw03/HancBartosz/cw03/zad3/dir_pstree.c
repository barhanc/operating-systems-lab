#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <string.h>

char resolved_path[PATH_MAX];
char pattern[255];

void traverse_dir(char *path)
{
    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        puts(path);
        perror("Error ocurred while opening directory!");
        exit(1);
        return;
    }

    struct dirent *dir;
    struct stat stat_buffer;
    pid_t child_pid;

    while ((dir = readdir(dirp)) != NULL)
    {
        char buff[2 * PATH_MAX] = "";
        strcat(buff, path);
        strcat(buff, "/");
        strcat(buff, dir->d_name);

        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
        {
            stat(buff, &stat_buffer);
            if (!S_ISDIR(stat_buffer.st_mode))
            {
                int infile = open(buff, O_RDONLY);
                char s[strlen(pattern) + 1];
                char c;
                int i = 0;
                while (read(infile, &c, 1) == 1 && i < strlen(pattern))
                {
                    s[i] = c;
                    i++;
                }
                s[strlen(pattern)] = '\0';
                close(infile);

                if (strcmp(s, pattern) == 0)
                {
                    printf("%s | PID: %d\n", buff, getpid());
                }
            }
            else
            {
                child_pid = vfork();
                if (child_pid == 0)
                {
                    traverse_dir(buff);
                    exit(0);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 1 + 2)
    {
        puts("Incorrect number of argument!");
        return 0;
    }
    if (realpath(argv[1], resolved_path) == NULL)
    {
        printf("Error ocurred while resolving path: %s\n", argv[1]);
        return 0;
    }
    strcpy(pattern, argv[2]);

    traverse_dir(resolved_path);

    return 0;
}