#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <json.h>

#include "wine.h"
#include "net.h"
#include "tar.h"
#include "config.h"
#include "common.h"

static const struct Command wine_commands[] = {
    { .name = "download",       .func = wine_download,  .description = "download and extract a wine versions" },
    { .name = "env",            .func = wine_env,       .description = "add wine to your PATH in a POSIX shell"},
    { .name = "env-fish",       .func = wine_env,       .description = "add wine to your PATH in the fish shell"},
    { .name = "list",           .func = wine_list,      .description = "list installable wine versions" },
    { .name = "list-installed", .func = wine_installed, .description = "list already installed wine versions" },
    { .name = "remove",         .func = wine_remove,    .description = "remove a wine version" },
    { .name = "run",            .func = wine_run,       .description = "run an installed wine version" },
};

static const struct Flag wine_flags[] = {
    { .name = "help", .variant = TWO, .func = wine_help, .description = "show this message"}
};

COMMAND_GROUP_FUNC(wine)

COMMAND(wine, download)
{
    if (argc >= 2)
    {
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
    }
    else
    {
        fprintf(stderr, USAGE_STR " wine download [versions]\n\nversions are obtained via '" NAME " wine list'\n");
    }
    return 0;
}


COMMAND(wine, remove)
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
                return 0;
            }
        }

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

    return 0;
}

COMMAND(wine, list)
{
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

    return 0;
}

COMMAND(wine, run)
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
            case WINE_NORMAL:
                winebinloc = WINEBIN;
                break;

            case WINE_PROTON:
                winebinloc = PROTONBIN;
                break;

            default:
                #ifdef DEBUG
                fprintf(stderr, "Couldn't find figure out if this '%s' is Wine or Proton, defaulting to Wine\n", winever);
                #endif
                winebinloc = WINEBIN;
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

    fprintf(stderr, "Specify a what wine version to run.\nUse '" NAME " wine list-installed' to list available versions\n");
        
    return 0;
}

COMMAND(wine, installed)
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

    return 0;
}

COMMAND(wine, env)
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
                return 0;
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
                #ifdef DEBUG
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
                    printf("$ eval 'polecat wine env %s'\n", winever);
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
            //printf("PATH=\"%s\"\n# Run this code in your Terminal\n# by running eval '%s'", newpath, argv[0]);
        }
        else
        {
            fprintf(stderr, "cannot find wine for '%s'\n", winever);
        }

    }
    else
    {
        fprintf(stderr, "Specify a what wine version to run.\nUse '" NAME " wine list-installed' to list available versions\n");
    }

        
    return 0;
}

COMMAND_HELP(wine, " wine")

enum wine_type_t check_wine_ver(char* winepath, size_t size)
{
    char* winepathcopy = NULL;

    winepathcopy = malloc(size);
    if (winepathcopy)
    {
        strncpy(winepathcopy, winepath, size);
        strncat(winepathcopy, WINEBIN, size - strlen(winepathcopy));

        if (isFile(winepathcopy))
        {
            free(winepathcopy);
            return WINE_NORMAL;
        }

        strncpy(winepathcopy, winepath, size);
        strncat(winepathcopy, PROTONBIN, size - strlen(winepathcopy));

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
