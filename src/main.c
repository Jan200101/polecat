#include <stdio.h>
#include <string.h>

#include "main.h"
#include "wine.h"
#include "dxvk.h"
#include "common.h"
#include "config.h"

const static struct Command main_commands[] = {
    { .name = "wine",     .func = wine,         .description = "manage wine versions" },
    { .name = "dxvk",     .func = dxvk,         .description = "manage dxvk versions (TODO)" },
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
    char cfgdir[256];
    char datadir[256];

    getConfigDir(cfgdir);
    getDataDir(datadir);

    printf("version:\t\t%s\n"
           "user-Agent:\t\t%s/%s\n"
           "config dir\t\t%s\n"
           "data dir\t\t%s\n",
           VERSION,
           NAME, VERSION,
           cfgdir, datadir);

    return 0;
}

int main_help(int argc, char** argv)
{
    puts("usage: polecat <command>\n\nList of commands:");

    print_help(main_commands, ARRAY_LEN(main_commands));

    return 0;
}