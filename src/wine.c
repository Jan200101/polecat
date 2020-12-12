
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <dirent.h>

#include "wine.h"
#include "net.h"
#include "tar.h"
#include "config.h"
#include "common.h"

const static struct Command wine_commands[] = {
    { .name = "download",       .func = wine_download,  .description = "download and extract a wine version" },
    { .name = "remove",         .func = wine_remove,    .description = "remove a wine version" },
    { .name = "list",           .func = wine_list,      .description = "list installable wine versions" },
    { .name = "run",            .func = wine_run,       .description = "run an installed wine version" },
    { .name = "list-installed", .func = wine_installed, .description = "list already installed wine versions" },
    { .name = "env",            .func = wine_env,       .description = "add wine to your PATH in a POSIX shell"},
    { .name = "fish-env",       .func = wine_env,       .description = "add wine to your PATH in the fish shell"},
};

COMMAND_GROUP_FUNC(wine)

COMMAND(wine, download)
{
    if (argc == 2)
    {
        struct json_object* runner = fetchJSON(WINE_API);

        if (runner)
        {
            struct json_object* versions, *value, *temp;
            uint8_t found = 0;
            json_object_object_get_ex(runner, "versions", &versions);

            char* choice = argv[1];

            for (int i = 0; i < json_object_array_length(versions); ++i)
            {
                value = json_object_array_get_idx(versions, i);
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
                
                fprintf(stderr, "Downloading %s\n", name);

                archive = downloadToRam(json_object_get_string(temp));
                if (archive)
                {
                    fprintf(stderr, "Extracting %s\n", name);
                    extract(archive, winedir);
                    fprintf(stderr, "Done\n");
                }
                else
                {
                    fprintf(stderr, "Something went wrong. The archive went missing\n");
                }

                free(archive->memory);
                free(archive);
            }
            else
            {
                fprintf(stderr, "Could not find `%s'\n", choice);
            }

            json_object_put(runner);
        }
    }
    else
    {
        fprintf(stderr, USAGE_STR " wine download <version>\n\nversions are obtained via `" NAME " wine list'\n");
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
                fprintf(stderr, "`%s' is not an installed wine version\n", winever);
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

    fprintf(stderr, USAGE_STR " wine remove <version>\n\nInstalled wine versions can be obtained by using `" NAME " wine list-installed\n");

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

        for (size_t i = 0; i < json_object_array_length(versions); ++i)
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
                fprintf(stderr, "`%s' is not an installed wine version\n", winever);
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
                fprintf(stderr, "Couldn't find figure out if this `%s' is Wine or Proton, defaulting to Wine\n", winever);
                #endif
                winebinloc = WINEBIN;
                break;
        }

        strncat(winepath, winebinloc, sizeof(winepath) - strlen(winepath) - 1);

        if (isFile(winepath))
        {
            for (int i = 2; i < argc; ++i)
            {
                strncat(winepath, " \"", sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, argv[i], sizeof(winepath) - strlen(winepath) - 1);
                strncat(winepath, "\"", sizeof(winepath) - strlen(winepath) - 1);
            }

            return system(winepath);
        }
        else
        {
            fprintf(stderr, "cannot find wine for `%s'\n", winever);
        }

    }

    fprintf(stderr, "Specify a what wine version to run.\nUse `" NAME " wine list-installed' to list available versions\n");
        
    return 0;
}

COMMAND(wine, installed)
{
    char winedir[PATH_MAX];
    getWineDir(winedir, sizeof(winedir));

    DIR *dir;
    struct dirent *ent;

    int intty = isatty(STDOUT_FILENO);

    if (intty) puts("Installed wine versions:");

    if ((dir = opendir(winedir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_name[0] != '.' && ent->d_type == DT_DIR)
            {
                if (intty) printf(" - ");
                printf("%s\n", ent->d_name);
            }
        }
        closedir (dir);
    } 

    return 0;
}

COMMAND(wine, env)
{
    if (argc > 1)
    {
        // instead of creating redundant copies we just check for fish
        bool fish_env = (strcmp(argv[0], "fish-env") == 0);

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
                fprintf(stderr, "`%s' is not an installed wine version\n", winever);
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
                fprintf(stderr, "Couldn't find figure out if this `%s' is Wine or Proton, defaulting to Wine", winever);
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
                       "you have to eval the output.\n\n"
                       "$ eval `polecat wine env %s`\n", winever);
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
            //printf("PATH=\"%s\"\n# Run this code in your Terminal\n# by running eval `%s`", newpath, argv[0]);
        }
        else
        {
            fprintf(stderr, "cannot find wine for `%s'\n", winever);
        }

    }
    else
    {
        fprintf(stderr, "Specify a what wine version to run.\nUse `" NAME " wine list-installed' to list available versions\n");
    }

        
    return 0;
}

COMMAND_HELP(wine, " wine");

enum wine_type_t check_wine_ver(char* winepath, size_t size)
{
    char* winepathcopy = NULL;

    winepathcopy = malloc(size);
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

    return WINE_NONE;
}
