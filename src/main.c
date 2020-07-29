#include <stdio.h>
#include <string.h>

#include "main.h"
#include "wine.h"
#include "dxvk.h"
#include "common.h"

const static struct Command main_commands[] = {
    { .name = "wine",     .func = wine,         .description = "manage wine versions" },
    { .name = "dxvk",     .func = dxvk,         .description = "manage dxvk versions" },
    { .name = "info",     .func = main_info,    .description = "show some information about polecat" },
    { .name = "help",     .func = main_help,    .description = "displays this message" },
};


int main(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 0; i < ARRAY_LEN(main_commands); ++i)
        {
            if (!strcmp(main_commands[i].name, argv[1])) return main_commands[i].func(argc, argv);
        }
    } 

    return main_help(argc, argv);
}

int main_info(int argc, char** argv)
{
    printf("Version:\t\t%s\n"
           "User-Agent:\t\t%s/%s\n",
           VERSION,
           NAME, VERSION);

    return 0;
}

int main_help(int argc, char** argv)
{
    puts("usage: polecat <command>\n\nList of commands:");

    print_help(main_commands, ARRAY_LEN(main_commands));

    return 0;
}