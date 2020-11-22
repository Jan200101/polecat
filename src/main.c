
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>

#include "main.h"
#include "wine.h"
#include "dxvk.h"
#include "lutris.h"
#include "common.h"
#include "config.h"

const static struct Command main_commands[] = {
    { .name = "wine",   .func = wine,      .description = "manage wine versions" },
    { .name = "dxvk",   .func = dxvk,      .description = "manage DXVK versions" },
    { .name = "lutris", .func = lutris,    .description = "run lutris instraller"},
    { .name = "env",    .func = main_env,  .description = "show some information about polecat" },
};


int main(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 0; i < ARRAY_LEN(main_commands); ++i)
        {
            if (!strcmp(main_commands[i].name, argv[1])) return main_commands[i].func(argc-1, argv+1);
        }
    } 

    return main_help(argc-1, argv+1);
}

int main_env(int argc, char** argv)
{
    char buffer[PATH_MAX];


    printf("version:\t\t%s\n"
           "user-Agent:\t\t%s\n",
           VERSION,
           USER_AGENT);

    getConfigDir(buffer, sizeof(buffer));
    printf("config dir\t\t%s\n", buffer);

    getDataDir(buffer, sizeof(buffer));
    printf("data dir\t\t%s\n", buffer);


    return 0;
}

int main_help(int argc, char** argv)
{
    puts(USAGE_STR " <command>\n\nList of commands:");

    print_help(main_commands, ARRAY_LEN(main_commands));

    return 0;
}