#include <polecat-config.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

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
    struct stat sb;

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

void makeDir(const char* path)
{
    struct stat st = {0};

    if (stat(path, &st) == -1)
    {
        mkdir(path, 0755);
    }
}

