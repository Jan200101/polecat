
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "main.h"
#include "wine.h"
#include "dxvk.h"
#include "lutris.h"
#include "common.h"
#include "config.h"

static const struct Command main_commands[] = {
#ifndef _WIN32
    { .name = "wine",   .func = wine,      .description = "manage wine versions" },
#endif
    { .name = "dxvk",   .func = dxvk,      .description = "manage DXVK versions" },
    { .name = "lutris", .func = lutris,    .description = "run lutris instraller"},
    { .name = "env",    .func = main_env,  .description = "show some information about polecat" },
};

static const struct Flag main_flags[] = {
    { .name = "help",    .variant = TWO,    .func = main_help,    .description = "show this message"},
    { .name = "version", .variant = BOTH,   .func = main_version, .description = "prints the program version"}
};

COMMAND_GROUP_FUNC(main)

COMMAND(main, env)
{
    char buffer[PATH_MAX];


    printf("user-Agent:\t\t%s\n", USER_AGENT);

    getConfigDir(buffer, sizeof(buffer));
    printf("config dir\t\t%s\n", buffer);

    getDataDir(buffer, sizeof(buffer));
    printf("data dir\t\t%s\n", buffer);


    return 0;
}

COMMAND(main, version)
{
    puts(VERSION);

    return 0;
}

COMMAND_HELP(main, "")
