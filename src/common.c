
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "common.h"

void print_help(const struct Command* commands, const size_t size)
{
    int longestCommand = 0;

    for (size_t i = 0; i < size; ++i)
    {
        int commandLength = strlen(commands[i].name);

        if (commandLength > longestCommand) longestCommand = commandLength;
    }

    for (size_t i = 0; i < size; ++i)
    {
        printf("\t%-*s\t %s\n", longestCommand, commands[i].name, commands[i].description);
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

bool isFile(const char* path)
{
    struct stat sb = getStat(path);

    return S_ISREG(sb.st_mode) || S_ISLNK(sb.st_mode);
}

bool isDir(const char* path)
{
    struct stat sb = getStat(path);

    return S_ISDIR(sb.st_mode) || S_ISLNK(sb.st_mode);
}

int makeDir(const char* path)
{
    char pathcpy[PATH_MAX];
    char *index;

    strncpy(pathcpy, path, PATH_MAX); // make a mutable copy of the path

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

int removeDir(const char *path) {
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
                    else if (S_ISLNK(statbuf.st_mode))
                        r = unlink(buf);
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