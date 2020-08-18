#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json.h>
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "wine.h"
#include "net.h"
#include "tar.h"
#include "common.h"
#include "config.h"


const static struct Command wine_commands[] = {
    { .name = "download",       .func = wine_download,  .description = "download and extract a wine version from lutris" },
    { .name = "list",           .func = wine_list,      .description = "list installable wine versions" },
    { .name = "run",            .func = wine_run,       .description = "run a installed wine version" },
    { .name = "installed",      .func = wine_installed, .description = "list installed wine versions" },
};

int wine(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 0; i < ARRAY_LEN(wine_commands); ++i)
        {
            if (!strcmp(wine_commands[i].name, argv[1])) return wine_commands[i].func(argc-1, argv+1);
        }
    } 


    return wine_help(argc, argv);
}

int wine_download(int argc, char** argv)
{
    if (argc == 2)
    {
        struct json_object* runner = fetchJSON(WINE_API);

        if (runner)
        {
            struct json_object* versions;
            json_object_object_get_ex(runner, "versions", &versions);

            int choice = atoi(argv[1]);

            if (choice > json_object_array_length(versions) - 1 || choice < 0)
            {
                printf("`%i' is not a valid ID\n\nrun `polecat wine list' to get a valid ID\n", choice);
            }
            else
            {
                struct json_object* url;
                struct json_object* value = json_object_array_get_idx(versions, choice);

                json_object_object_get_ex(value, "url", &url);
                char* name = basename((char*)json_object_get_string(url));

                char datadir[PATH_MAX];
                char downloadpath[PATH_MAX];
                struct MemoryStruct* tar;

                getDataDir(datadir, sizeof(datadir));
                makeDir(datadir);

                strncpy(downloadpath, datadir, sizeof(downloadpath));
                strncat(downloadpath, "/", sizeof(downloadpath) - strlen(downloadpath));
                strncat(downloadpath, name, sizeof(downloadpath) - strlen(downloadpath));
                
                printf("Downloading %s\n", name);

                tar = downloadToRam(json_object_get_string(url));
                if (tar)
                {
                    printf("Extracting %s\n", name);
                    extract(tar, datadir);
                    puts("Done");
                }
                else
                {
                    puts("Something went wrong. The tar is not valid");
                }


                free(tar->memory);
                free(tar);
            }

            json_object_put(runner);
        }
    }
    else
    {
        puts(USAGE_STR " wine download <ID>\n\nIDs are obtained via `" NAME " wine list' ");
    }
    return 0;
}

int wine_list(int argc, char** argv)
{
    struct json_object* runner = fetchJSON(WINE_API);

    if (runner)
    {
        struct json_object* versions, *value, *val;
        json_object_object_get_ex(runner, "versions", &versions);

        puts("Installable wine versions:");

        for (size_t i = 0; i < json_object_array_length(versions); ++i)
        {
            value = json_object_array_get_idx(versions, i);
            json_object_object_get_ex(value, "version", &val);
            printf(" [%zu]\t%s\n", i, json_object_get_string(val));
        }

        json_object_put(runner);
    }

    return 0;
}

int wine_run(int argc, char** argv)
{
    if (argc > 1)
    {
        char winepath[PATH_MAX];
        getDataDir(winepath, sizeof(winepath));
        char* winever = argv[1];

        strcat(winepath, "/");
        strcat(winepath, winever);
        strcat(winepath, "/bin/wine");

        for (int i = 2; i < argc; ++i)
        {
            strcat(winepath, " ");
            strcat(winepath, argv[i]);
        }

        return system(winepath);
    }

    printf("Specify a what wine version to run.\nUse `" NAME " wine list-installed' to list available versions\n");
        
    return 0;
}

int wine_installed(int argc, char** argv)
{
    char datadir[PATH_MAX];
    getDataDir(datadir, sizeof(datadir));

    DIR *dir;
    struct dirent *ent;

    printf("Installed wine versions:\n");
    if ((dir = opendir(datadir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            /*
             * WARNING: crusty
             * d_type is only specified on glibc (including musl) and BSD
             */
            if (ent->d_name[0] != '.' && ent->d_type == DT_DIR)
            {
                printf(" - %s\n", ent->d_name);
            }
        }
        closedir (dir);
    } 

    return 0;
}

int wine_help(int argc, char** argv)
{
    puts(USAGE_STR " wine <command>\n\nList of commands:");

    print_help(wine_commands, ARRAY_LEN(wine_commands));

    return 0;
}