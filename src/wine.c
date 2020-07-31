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
    { .name = "download",       .func = wine_download,   .description = "download and extract a wine version from lutris" },
    { .name = "list",           .func = wine_list,       .description = "list installable wine versions" },
    { .name = "run",            .func = wine_run,        .description = "run a installed wine version" },
    { .name = "list-installed", .func = wine_installed,  .description = "list installed wine versions" },
    { .name = "help",           .func = wine_help,       .description = "shows this message" },
};

int wine(int argc, char** argv)
{
    if (argc > 2)
    {
        for (int i = 0; i < ARRAY_LEN(wine_commands); ++i)
        {
            if (!strcmp(wine_commands[i].name, argv[2])) return wine_commands[i].func(argc, argv);
        }
    } 

    return wine_help(argc, argv);
}

int wine_download(int argc, char** argv)
{
    if (argc == 4)
    {
        struct json_object* runner = fetchJSON(WINE_API);

        if (runner)
        {
            struct json_object* versions;
            json_object_object_get_ex(runner, "versions", &versions);

            int choice = atoi(argv[3]);

            if (choice > json_object_array_length(versions) - 1 || choice < 0)
            {
                fprintf(stderr, "`%i' is not a valid ID\n\nrun `polecat wine list' to get a valid ID\n", choice);
            }
            else
            {
                struct json_object* url;
                struct json_object* value = json_object_array_get_idx(versions, choice);

                json_object_object_get_ex(value, "url", &url);
                char* name = basename((char*)json_object_get_string(url));

                char datadir[PATH_MAX];
                char downloadpath[PATH_MAX];

                getDataDir(datadir);
                makeDir(datadir);

                strcpy(downloadpath, datadir);
                strcat(downloadpath, "/");
                strcat(downloadpath, name);
                
                fprintf(stderr, "Downloading %s\n", name);
                downloadFile(json_object_get_string(url), downloadpath);
                fprintf(stderr, "Extracting %s\n", name);
                extract(downloadpath, datadir);
                fprintf(stderr, "Done\n");
            }
        }
    }
    else
    {
        puts("Usage: " NAME " wine download <ID>\n\nIDs are obtained via `" NAME " wine list' ");
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
    }

    return 0;
}

int wine_run(int argc, char** argv)
{
    if (argc > 3)
    {
        char winepath[PATH_MAX];
        getDataDir(winepath);
        char* winever = argv[3];

        strcat(winepath, "/");
        strcat(winepath, winever);
        strcat(winepath, "/bin/wine");

        for (int i = 4; i < argc; ++i)
        {
            strcat(winepath, " ");
            strcat(winepath, argv[i]);
        }

        return system(winepath);
    }
    else
    {
        fprintf(stderr, "Specify a what wine version to run.\nUse `" NAME " wine list-installed' to list available versions\n");
    }
        
    return 0;
}

int wine_installed(int argc, char** argv)
{
    char datadir[PATH_MAX];
    getDataDir(datadir);

    DIR *dir;
    struct dirent *ent;

    fprintf(stderr, "Installed wine versions:\n");
    if ((dir = opendir(datadir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_name[0] != '.' && ent->d_type == 4)
            {
                fprintf(stderr, " - %s\n", ent->d_name);
            }
        }
        closedir (dir);
    } 

    return 0;
}

int wine_help(int argc, char** argv)
{
    puts("usage: " NAME " wine <command>\n\nList of commands:");

    print_help(wine_commands, ARRAY_LEN(wine_commands));

    return 0;
}