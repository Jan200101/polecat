#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <json.h>

#include "common.h"
#include "config.h"
#include "fs.h"
#include "net.h"
#include "tar.h"
#include "wine.h"

static const struct Command winecmd_commands[] = {
    { .name = "download",       .func = winecmd_download,  .description = "download and extract a wine version" },
    { .name = "env",            .func = winecmd_env,       .description = "add wine to your PATH in a POSIX shell"},
    { .name = "env-fish",       .func = winecmd_env,       .description = "add wine to your PATH in the fish shell"},
    { .name = "installed",      .func = winecmd_installed, .description = "list installed wine versions" },
    { .name = "list",           .func = winecmd_list,      .description = "list available wine versions" },
    { .name = "remove",         .func = winecmd_remove,    .description = "remove a wine version" },
    { .name = "run",            .func = winecmd_run,       .description = "run an installed wine version" },
    { .name = "shim",           .func = winecmd_shim,      .description = "add a wine shim to your users bin folder" },
};

static const struct Flag winecmd_flags[] = {
    { .name = "help",       .variant = TWO, .returns = 1, .func = winecmd_help, .description = "show this message"},
    { .name = "no-net",     .variant = TWO, .returns = 0, .func = set_no_net,   .description = "run commands without commitment"}
};

COMMAND_GROUP_FUNC(winecmd)

COMMAND(winecmd, download)
{
    if (argc >= 2)
    {
        net_init();
        struct json_object* runner = fetchJSON(WINE_API);

        if (runner)
        {
            struct json_object* versions, *value, *temp;
            int found;
            json_object_object_get_ex(runner, "versions", &versions);

            for (int i = 1; i < argc; ++i)
            {
                found = 0;
                char* choice = argv[i];


                for (JSON_LENGTH_TYPE j = 0; j < json_object_array_length(versions); ++j)
                {
                    value = json_object_array_get_idx(versions, j);
                    json_object_object_get_ex(value, "version", &temp);
                    if (strcmp(json_object_get_string(temp), choice) == 0)
                    {
                        found = 1;
                        break;
                    }
                }

                if (found)
                {
                    json_object_object_get_ex(value, "url", &temp);

                    char* name = basename((char*)json_object_get_string(temp));

                    char winedir[PATH_MAX];
                    struct MemoryStruct* archive;

                    getWineDir(winedir, sizeof(winedir));
                    makeDir(winedir);

                    fprintf(stderr, "Downloading %s...\n", name);

                    archive = downloadToRam(json_object_get_string(temp), 1);
                    if (archive)
                    {
                        fprintf(stderr, "Extracting %s\n", name);
                        extract(archive, winedir);
                        fprintf(stderr, "Done\n");

                        free(archive->memory);
                        free(archive);
                    }
                    else
                    {
                        fprintf(stderr, "Something went wrong. The archive went missing\n");
                    }

                }
                else
                {
                    fprintf(stderr, "Could not find '%s'\n", choice);
                }
            }

            json_object_put(runner);
        }
        net_deinit();
    }
    else
    {
        fprintf(stderr, USAGE_STR " wine download [versions]\n\nversions are obtained via '" NAME " wine list'\n");
    }

    return EXIT_SUCCESS;
}


COMMAND(winecmd, remove)
{
    if (argc == 2)
    {
        char* winever = argv[1];

        char winepath[PATH_MAX];
        getWineDir(winepath, sizeof(winepath));

        strncat(winepath, "/", sizeof(winepath) - strlen(winepath) - 1);
        strncat(winepath, winever, sizeof(winepath) - strlen(winepath) - 1);

        if (!isDir(winepath))
        {

            // if the wine version does not exist try appending the system arch e.g. x86_64
            struct utsname buffer;

            if (!uname(&buffer))
            {
                strncat(winepath, "-", sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, buffer.machine, sizeof(winepath) - strlen(winepath) - 1);
            }

            // if it still doesn't exist tell this wine version is not installed
            if (!isDir(winepath))
            {
                fprintf(stderr, "'%s' is not an installed wine version\n", winever);
                return EXIT_FAILURE;
            }
        }
	puts(winepath);

        int retval = removeDir(winepath);
        if (!retval)
        {
            fprintf(stderr, "Done\n");
        }
        else
        {
            fprintf(stderr, "Something might have gone wrong. Manual cleanup might be required\n");
        }

        return retval;
    }

    fprintf(stderr, USAGE_STR " wine remove <version>\n\nInstalled wine versions can be obtained by using '" NAME " wine list-installed\n");

    return EXIT_SUCCESS;
}

COMMAND(winecmd, list)
{
    net_init();
    struct json_object* runner = fetchJSON(WINE_API);

    if (runner)
    {
        struct json_object* versions, *value, *val;
        json_object_object_get_ex(runner, "versions", &versions);

        int intty = isatty(STDOUT_FILENO);

        if(intty) puts("Installable wine versions:");

        for (JSON_LENGTH_TYPE i = 0; i < json_object_array_length(versions); ++i)
        {
            value = json_object_array_get_idx(versions, i);
            json_object_object_get_ex(value, "version", &val);
            if (intty) printf(" - ");
            printf("%s\n", json_object_get_string(val));
        }

        json_object_put(runner);
    }

    net_deinit();
    return EXIT_SUCCESS;
}

COMMAND(winecmd, run)
{
    if (argc > 1)
    {
        char winepath[PATH_MAX];
        char* winebinloc = NULL; // to be set by the wine type check
        getWineDir(winepath, sizeof(winepath));
        char* winever = argv[1];

        strncat(winepath, "/", sizeof(winepath) - strlen(winepath) - 1);
        strncat(winepath, winever, sizeof(winepath) - strlen(winepath) - 1);

        if (!isDir(winepath))
        {

            // try appending the system arch to find the wine version
            struct utsname buffer;

            if (!uname(&buffer))
            {
                strncat(winepath, "-", sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, buffer.machine, sizeof(winepath) - strlen(winepath) - 1);
            }

            // if we still cannot find anything tell the user and exit
            if (!isDir(winepath))
            {
                fprintf(stderr, "'%s' is not an installed wine version\n", winever);
                return 1;
            }
        }

        switch(check_wine_ver(winepath, sizeof(winepath)+1))
        {
            default:
            case WINE_NORMAL:
                winebinloc = WINEBIN;
                break;

            case WINE_PROTON:
                winebinloc = PROTONBIN;
                break;
        }

        strncat(winepath, winebinloc, sizeof(winepath) - strlen(winepath) - 1);

        if (isFile(winepath))
        {
            argv[1] = winepath;
            return execvp(winepath, argv+1);
        }
        else
        {
            fprintf(stderr, "cannot find wine for '%s'\n", winever);
        }

    }
    else
    {
        fprintf(stderr, USAGE_STR " wine run [version] <options>\nUse '" NAME " wine list-installed' to list available versions\n");
    }


    return EXIT_SUCCESS;
}

COMMAND(winecmd, installed)
{
    char winedir[PATH_MAX];
    getWineDir(winedir, sizeof(winedir));

    size_t winelen = strlen(winedir)+1;
    winedir[winelen-1] = '/';

    DIR *dir;
    struct dirent *ent;

    int intty = isatty(STDOUT_FILENO);

    if (intty) puts("Installed wine versions:");

    if ((dir = opendir(winedir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_name[0] == '.') continue;
            strncat(winedir, ent->d_name, sizeof(winedir) - winelen - 1);
            int isdirec = isDir(winedir);
            winedir[winelen] = '\0';

            if (!isdirec) continue;

            if (intty) printf(" - ");
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    }

    return EXIT_SUCCESS;
}

COMMAND(winecmd, env)
{
    if (argc > 1)
    {
        // instead of creating redundant copies we just check for fish
        int fish_env = (strcmp(argv[0], "env-fish") == 0);

        char winepath[PATH_MAX];
        char* winebinloc = NULL; // to be set by the wine type check
        getWineDir(winepath, sizeof(winepath));
        char* winever = argv[1];

        strncat(winepath, "/", sizeof(winepath) - strlen(winepath) - 1);
        strncat(winepath, winever, sizeof(winepath) - strlen(winepath) - 1);

        if (!isDir(winepath))
        {

            // if the wine version does not exist try appending the system arch e.g. x86_64
            struct utsname buffer;

            if (!uname(&buffer))
            {
                strncat(winepath, "-", sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, buffer.machine, sizeof(winepath) - strlen(winepath) - 1);
            }

            // if it still doesn't exist tell this wine version is not installed
            if (!isDir(winepath))
            {
                fprintf(stderr, "'%s' is not an installed wine version\n", winever);
                return EXIT_FAILURE;
            }
        }

        switch(check_wine_ver(winepath, sizeof(winepath)+1))
        {
            case WINE_NORMAL:
                winebinloc = WINEBIN;
                break;

            case WINE_PROTON:
                winebinloc = PROTONBIN;
                break;

            default:
                #ifndef NDEBUG
                fprintf(stderr, "Couldn't find figure out if this '%s' is Wine or Proton, defaulting to Wine", winever);
                #endif
                winebinloc = WINEBIN;
                break;
        }

        strncat(winepath, winebinloc, sizeof(winepath) - strlen(winepath) - 1);

        if (isFile(winepath))
        {
            winepath[strlen(winepath) - strlen("wine")] = '\0';
            if (isatty(STDOUT_FILENO))
            {
                printf("To add a wine installation to your PATH\n"
                       "you have to eval the output.\n\n");
                if (!fish_env)
                    printf("$ eval `polecat wine env %s`\n", winever);
                else
                    printf("$ eval (polecat wine fish-env %s)\n", winever);
            }
            else
            {
                if (!fish_env)
                {
                    printf("PS1=\"(%s) $PS1\"\nPATH=\"%s:$PATH\"\n", winever, winepath);
                }
                else
                {
                    printf("set PATH %s $PATH\n", winepath);
                }
            }
        }
        else
        {
            fprintf(stderr, "cannot find wine for '%s'\n", winever);
        }

    }
    else
    {
        fprintf(stderr, USAGE_STR " wine env [version] <options>\nUse '" NAME " wine list-installed' to list available versions\n");
    }

    return EXIT_SUCCESS;
}

COMMAND(winecmd, shim)
{
    if (argc > 1)
    {
        char winepath[PATH_MAX];
        char* winebinloc = NULL; // to be set by the wine type check
        getWineDir(winepath, sizeof(winepath));
        char* winever = argv[1];

        strncat(winepath, "/", sizeof(winepath) - strlen(winepath) - 1);
        strncat(winepath, winever, sizeof(winepath) - strlen(winepath) - 1);

        if (!isDir(winepath))
        {

            // try appending the system arch to find the wine version
            struct utsname buffer;

            if (!uname(&buffer))
            {
                strncat(winepath, "-", sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, buffer.machine, sizeof(winepath) - strlen(winepath) - 1);
            }

            // if we still cannot find anything tell the user and exit
            if (!isDir(winepath))
            {
                fprintf(stderr, "'%s' is not an installed wine version\n", winever);
                return 1;
            }
        }

        switch(check_wine_ver(winepath, sizeof(winepath)+1))
        {
            default:
            case WINE_NORMAL:
                winebinloc = WINEBIN;
                break;

            case WINE_PROTON:
                winebinloc = PROTONBIN;
                break;
        }

        strncat(winepath, winebinloc, sizeof(winepath) - strlen(winepath) - 1);

        if (isFile(winepath))
        {
            char buffer[BUFSIZ];
            char binpath[PATH_MAX];

            readlink("/proc/self/exe", buffer, BUFSIZ);
            getUserBinDir(binpath, sizeof(binpath));
            strcat(binpath, "/wine-");
            strcat(binpath, winever);

            symlink(buffer, binpath);
            printf("Installed '%s' as '%s'´\n", winever, binpath);
        }
        else
        {
            fprintf(stderr, "cannot find wine for '%s'\n", winever);
        }

    }
    else
    {
        fprintf(stderr, USAGE_STR " wine shim [version]\n");
    }

    return EXIT_SUCCESS;
}

COMMAND_HELP(winecmd, " wine")

enum wine_type_t check_wine_ver(char* winepath, size_t size)
{
    char* winepathcopy = NULL;

    winepathcopy = malloc(size);
    if (winepathcopy)
    {
        strncpy(winepathcopy, winepath, size - 1);
        strncat(winepathcopy, WINEBIN, size - strlen(winepathcopy) - 1);

        if (isFile(winepathcopy))
        {
            free(winepathcopy);
            return WINE_NORMAL;
        }

        strncpy(winepathcopy, winepath, size - 1);
        strncat(winepathcopy, PROTONBIN, size - strlen(winepathcopy) - 1);

        if (isFile(winepathcopy))
        {
            free(winepathcopy);
            return WINE_PROTON;
        }

        free(winepathcopy);
    }
    else
    {
        fprintf(stderr, "Unable to allocate memory\n");
    }

    return WINE_NONE;
}
