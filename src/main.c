#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include "main.h"
#ifdef WINE_ENABLED
#include <alloca.h>
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
    { .name = "license", .variant = TWO,    .returns = 1,   .func = main_license, .description = "display the software license"},
    { .name = "version", .variant = BOTH,   .returns = 1,   .func = main_version, .description = "prints the program version"}
};

COMMAND_GROUP(main)
{
#ifdef WINE_ENABLED
    /*
     * if ran binary is starts with wine-
     * its likely that it ends with a wine version
     * so we rewrite the argv from
     * `wine-6.0` to `wine-6.0 wine run 6.0`
     * which will follow the command path down for
     * running wine.
     *
     * Since we run wine by replacing ourselves
     * this leaves a seamless experience
     * where the launched PID ends up as wine
     */

    char* arg0 = basename(argv[0]);
    if (!strncmp(WINE_PREFIX, arg0, strlen(WINE_PREFIX)))
    {
        int nargc = argc + 3;
        char** nargv = alloca((size_t)(nargc+1) * sizeof(char*));

        nargv[0] = argv[0];
        nargv[1] = "wine";
        nargv[2] = "run";
        nargv[3] = arg0 + strlen(WINE_PREFIX);
        for (int i = 1; i < argc; ++i) nargv[3+i] = argv[i];
        nargv[nargc] = NULL;

        argc = nargc;
        argv = nargv;
    }
#endif
    COMMAND_GROUP_BODY(main)
}

COMMAND(main, env)
{
    char buffer[PATH_MAX];

    printf("User-Agent\t\t%s\n", USER_AGENT);

    /* the config dir is unused for now */
    /*getConfigDir(buffer, sizeof(buffer));*/
    /*printf("config directory\t\t%s\n", buffer);*/

    getDataDir(buffer, sizeof(buffer));
    printf("data directory\t%s\n", buffer);


    return EXIT_SUCCESS;
}

COMMAND(main, license)
{
    puts(
        "\t" NAME ", a wine version manager\n"
        "\tCopyright (C) 2020-2022  Jan Drögehoff\n"
        "\n"
        "\tThis program is free software: you can redistribute it and/or modify\n"
        "\tit under the terms of the GNU General Public License as published by\n"
        "\tthe Free Software Foundation, either version 3 of the License, or\n"
        "\t(at your option) any later version.\n"
        "\n"
        "\tThis program is distributed in the hope that it will be useful,\n"
        "\tbut WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "\tMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "\tGNU General Public License for more details.\n"
        "\n"
        "\tYou should have received a copy of the GNU General Public License\n"
        "\talong with this program.  If not, see <https://www.gnu.org/licenses/>."
    );

    return EXIT_SUCCESS;
}

COMMAND(main, version)
{
    puts(VERSION);
    return EXIT_SUCCESS;   
}

COMMAND_HELP(main, "")
