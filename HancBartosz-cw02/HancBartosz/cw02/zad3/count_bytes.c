#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

int main()
{
    DIR *dirp = opendir("./");
    if (dirp == NULL)
    {
        puts("Error ocurred while opening directory!");
        return 0;
    }

    struct dirent *dir;
    struct stat stat_buffer;
    long long tot_size = 0;

    while ((dir = readdir(dirp)) != NULL)
    {
        stat(dir->d_name, &stat_buffer);

        if (!S_ISDIR(stat_buffer.st_mode))
        {
            printf("%ld %s\n", stat_buffer.st_size, dir->d_name);
            tot_size += stat_buffer.st_size;
        }
    }

    printf("%lld total\n", tot_size);

    return 0;
}