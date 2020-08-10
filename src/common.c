#include <stdio.h>
#include <string.h>

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
