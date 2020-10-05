#include <polecat-config.h>

#include <stdio.h>
#include <string.h>
#include <json.h>
#include <libgen.h>
#include <linux/limits.h>

#include "dxvk.h"
#include "net.h"
#include "tar.h"
#include "common.h"
#include "config.h"

const static struct Command dxvk_commands[] = {
    { .name = "install",      .func = dxvk_install,    .description = "download and install a dxvk version" },
    { .name = "list",         .func = dxvk_list,       .description = "list available dxvk versions" },
};

int dxvk(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 0; i < ARRAY_LEN(dxvk_commands); ++i)
        {
            if (!strcmp(dxvk_commands[i].name, argv[1])) return dxvk_commands[i].func(argc-1, argv+1);
        }
    } 

    return dxvk_help(argc, argv);
}


int dxvk_install(int argc, char** argv)
{
    if (argc == 2)
    {
        struct json_object* runner = fetchJSON(DXVK_API);

        if (runner)
        {

            int choice = atoi(argv[1]);

            if (choice > json_object_array_length(runner) - 1 || choice < 0)
            {
                printf("`%i' is not a valid ID\n\nrun `" NAME " dxvk list' to get a valid ID", choice);
            }
            else
            {
                struct json_object* version = json_object_array_get_idx(runner, choice);
                struct json_object* assets;

                json_object_object_get_ex(version, "assets", &assets);
                version =json_object_array_get_idx(assets, 0);

                json_object_object_get_ex(version, "browser_download_url", &assets);

                char* name = basename((char*)json_object_get_string(assets));
                struct MemoryStruct* tar;

                char datadir[PATH_MAX];
                getDataDir(datadir, sizeof(datadir));
                makeDir(datadir);
                
                printf("Downloading %s", json_object_get_string(assets));
                //downloadFile(json_object_get_string(assets), name);
                tar = downloadToRam(json_object_get_string(assets));

                printf("Extracting %s\n", name);

                extract(tar, datadir);

                printf("Done\n");

                free(tar->memory);
                free(tar);
            }

            json_object_put(runner);
        }
    }
    else
    {
        puts(USAGE_STR " dxvk download <ID>\n\nIDs are obtained via `" NAME " dxvk list' ");
    }
    return 0;
}

int dxvk_list(int argc, char** argv)
{
    struct json_object* runner = fetchJSON(DXVK_API);

    if (runner)
    {
        puts("Installable DXVK versions:");

        for (size_t i = 0; i < json_object_array_length(runner); ++i)
        {
            struct json_object* version = json_object_array_get_idx(runner, i);
            struct json_object* name;

            json_object_object_get_ex(version, "name", &name);
            printf(" [%zu]\t%s\n", i, json_object_get_string(name));
        }
    }

    return 0;
}

int dxvk_help(int argc, char** argv)
{
    puts(USAGE_STR " dxvk <command>\n\nList of commands:");

    print_help(dxvk_commands, ARRAY_LEN(dxvk_commands));

    return 0;
}