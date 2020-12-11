
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


COMMAND_GROUP_FUNC(main)

COMMAND(main, env)
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

COMMAND_HELP(main, "")