#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "common.h"

void print_help(const struct Command* commands, const size_t commands_size,
                const struct Flag* flags, size_t flags_size)
{
    size_t longestStr;
    size_t length;

    if (commands_size)
    {
        longestStr = 0;

        for (size_t i = 0; i < commands_size; ++i)
        {
            length = strlen(commands[i].name);

            if (length > longestStr) longestStr = length;
        }

        fprintf(stderr, "\nList of commands:\n");
        for (size_t i = 0; i < commands_size; ++i)
        {
            fprintf(stderr, "\t%-*s\t %s\n", (int)longestStr, commands[i].name, commands[i].description);
        }
    }


    if (flags_size)
    {
        longestStr = 0;

        for (size_t i = 0; i < flags_size; ++i)
        {
            length = strlen(flags[i].name);

            if (length > longestStr) longestStr = length;
        }

        fprintf(stderr, "\nList of flags:\n");
        for (size_t i = 0; i < flags_size; ++i)
        {
            fprintf(stderr, "\t");
            if (flags[i].variant & ONE)
                fprintf(stderr, "-%c", flags[i].name[0]);
            else
                fprintf(stderr, "   ");

            if (flags[i].variant & TWO)
            {
                if (flags[i].variant & ONE) fprintf(stderr, ",");
                fprintf(stderr, " --%-*s", (int)longestStr, flags[i].name);
            }

            fprintf(stderr, "\t %s\n", flags[i].description);
        }
    }
}

struct stat getStat(const char* path)
{
    // fill with 0s by default in the case stat fails
    struct stat sb = {0};

    // the return value signifies if stat failes (e.g. file not found)
    // unimportant for us if it fails it won't touch sb
    stat(path, &sb);

    return sb;
}

int isFile(const char* path)
{
    struct stat sb = getStat(path);

    return S_ISREG(sb.st_mode)
#ifndef _WIN32
           || S_ISLNK(sb.st_mode)
#endif
            ;
}

int isDir(const char* path)
{
    struct stat sb = getStat(path);

    return S_ISDIR(sb.st_mode)
#ifndef _WIN32
           || S_ISLNK(sb.st_mode)
#endif
            ;
}

int makeDir(const char* path)
{
    char pathcpy[PATH_MAX];
    char *index;

    strncpy(pathcpy, path, PATH_MAX-1); // make a mutable copy of the path

    for(index = pathcpy+1; *index; ++index)
    {

        if (*index == '/')
        {
            *index = '\0';

            if (mkdir(pathcpy, 0755) != 0)
            {
                if (errno != EEXIST)
                    return -1;
            }

            *index = '/';
        }
    }

    return mkdir(path, 0755);
}

int removeDir(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            char *buf;
            size_t len;

            // Skip the names "." and ".." as we don't want to recurse on them.
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf = {0};

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r = removeDir(buf);
#ifndef _WIN32
                    else if (S_ISLNK(statbuf.st_mode))
                        r = unlink(buf);
#endif
                    else
                        r = remove(buf);
                }
                else // it is very likely that we found a dangling symlink which is not detected by stat
                {
                    r = unlink(buf);
                }
                free(buf);
            }
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}
