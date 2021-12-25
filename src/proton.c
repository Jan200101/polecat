#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include "proton.h"
#include "config.h"

static const struct Command proton_commands[] = {
    { .name = "download",   .func = proton_download,    .description = "download and extract a third.party Proton release" },
    { .name = "list",       .func = proton_list,        .description = "list installable Proton version" },
    { .name = "installed",  .func = proton_installed,   .description = "list already installed Proton versions" },
    { .name = "remove",     .func = proton_remove,      .description = "remove a Proton release" },
};

static const struct Flag proton_flags[] = {
    { .name = "no-net",     .variant = TWO, .returns = 0, .func = set_no_net,   .description = "run commands without commitment"}
};

char* repos[]  = {
    GITHUB_RELEASE("GloriousEggroll/proton-ge-custom"),
    GITHUB_RELEASE("Frogging-Family/wine-tkg-git"),
    NULL
};

COMMAND_GROUP_FUNC(proton)

COMMAND(proton, download)
{
    return 0;
}

COMMAND(proton, list)
{
    char** repo_index = repos;

    do
    {
        puts(*repo_index);
        ++repo_index;
    }
    while(*repo_index);

    return 0;
}

COMMAND(proton, installed)
{
    char protondir[PATH_MAX];
    getProtonDir(protondir, sizeof(protondir));

    size_t protonlen = strlen(protondir)+1;
    protondir[protonlen-1] = '/';

    DIR *dir;
    struct dirent *ent;

    int intty = isatty(STDOUT_FILENO);

    if (intty) puts("Installed Proton versions:");

    if ((dir = opendir(protondir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_name[0] == '.') continue;
            strncat(protondir, ent->d_name, sizeof(protondir) - protonlen - 1);
            int isdirec = isDir(protondir);
            protondir[protonlen] = '\0';

            if (!isdirec) continue;

            if (intty) printf(" - ");
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    }

    return EXIT_SUCCESS;
}

COMMAND(proton, remove)
{
    return 0;
}


COMMAND_HELP(proton, " proton")
