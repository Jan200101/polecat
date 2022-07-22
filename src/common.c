#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "common.h"

uint8_t no_net = 0;

int set_no_net(UNUSED int argc, UNUSED char** argv)
{
#ifndef NDEBUG
    puts("[NO_NET] set");
#endif
    no_net = 1;
    return EXIT_SUCCESS;
}

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
