#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

long long tot_size = 0;

int fn(const char *fpath, const struct stat *sb, int typeflag)
{
    if (!S_ISDIR(sb->st_mode))
    {
        printf("%ld %s\n", sb->st_size, fpath);
        tot_size += sb->st_size;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 1 + 1)
    {
        puts("Incorrect number of arguments!");
        return 0;
    }

    DIR *dirp = opendir(argv[1]);
    if (dirp == NULL)
    {
        puts("Error ocurred while opening directory!");
        return 0;
    }

    int (*fn_ptr)(const char *, const struct stat *, int) = &fn;
    ftw(argv[1], fn_ptr, 0);

    printf("%lld total\n", tot_size);

    return 0;
}