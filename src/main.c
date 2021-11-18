#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>

#include "main.h"
#ifdef WINE_ENABLED
#include "wine.h"
#endif
#ifdef DXVK_ENABLED
#include "dxvk.h"
#endif
#ifdef LUTRIS_ENABLED
#include "lutris.h"
#endif
#include "common.h"
#include "config.h"

#ifdef WINE_ENABLED
// if something fails
// we need to free the new argv
char** nargv;
static void free_nargv() { free(nargv); }
#endif

static const struct Command main_commands[] = {
#ifdef WINE_ENABLED
    { .name = "wine",    .func = winecmd,   .description = "manage wine versions" },
#endif
#ifdef DXVK_ENABLED
    { .name = "dxvk",    .func = dxvk,      .description = "manage DXVK versions" },
#endif
#ifdef LUTRIS_ENABLED
    { .name = "lutris",  .func = lutris,    .description = "run lutris instraller"},
#endif
    { .name = "env",     .func = main_env,  .description = "show some information about polecat" },
};

static const struct Flag main_flags[] = {
    { .name = "help",    .variant = TWO,    .returns = 1,   .func = main_help,    .description = "show this message"},
    { .name = "version", .variant = BOTH,   .returns = 1,   .func = main_version, .description = "prints the program version"}
};

COMMAND_GROUP(main)
{
#ifdef WINE_ENABLED
    char* arg0 = basename(argv[0]);
    if (!strncmp(WINE_PREFIX, arg0, strlen(WINE_PREFIX)))
    {
        int nargc = argc + 3;
        nargv = malloc((size_t)(nargc+1) * sizeof(char*));

        nargv[0] = argv[0];
        nargv[1] = "wine";
        nargv[2] = "run";
        nargv[3] = arg0 + strlen(WINE_PREFIX);
        for (int i = 1; i < argc; ++i) nargv[3+i] = argv[i];
        nargv[nargc] = NULL;

        argc = nargc;
        argv = nargv;

        /* we cannot free nargv before the body parsed it
         * and we cannot free it after because the body
         * returns, so call this at exit to free the leak
         */
        atexit(free_nargv);
    }
#endif
    COMMAND_GROUP_BODY(main)
}

COMMAND(main, env)
{
    char buffer[PATH_MAX];

    printf("user-Agent:\t\t%s\n", USER_AGENT);

    getConfigDir(buffer, sizeof(buffer));
    printf("config dir\t\t%s\n", buffer);

    getDataDir(buffer, sizeof(buffer));
    printf("data dir\t\t%s\n", buffer);


    return EXIT_SUCCESS;
}

COMMAND(main, version)
{
    puts(VERSION);

    return EXIT_SUCCESS;
}

COMMAND_HELP(main, "")
