#include <stdio.h>
#include <string.h>
#include <json.h>
#include <libgen.h>

#include "wine.h"
#include "net.h"
#include "common.h"

const static struct Command wine_commands[] = {
    { .name = "install",      .func = wine_install,    .description = "download and install a wine version from lutris" },
    { .name = "list",         .func = wine_list,       .description = "list available wine versions" },
    { .name = "help",         .func = wine_help,       .description = "shows this message" },
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


int wine_install(int argc, char** argv)
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
                printf("`%i' is not a valid ID\n\nrun `polecat wine list' to get a valid ID", choice);
            }
            else
            {
                struct json_object* url;
                struct json_object* value = json_object_array_get_idx(versions, choice);

                json_object_object_get_ex(value, "url", &url);
                char* name = basename((char*)json_object_get_string(url));
                
                printf("Downloading %s", name);
                downloadFile(json_object_get_string(url), name);
                printf("\nDone\n");
            }
        }
    }
    else
    {
        puts("Usage: polecat wine download <ID>\n\nIDs are obtained via `polecat wine list' ");
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

int wine_help(int argc, char** argv)
{
    puts("usage: polecat wine <command>\n\nList of commands:");

    print_help(wine_commands, ARRAY_LEN(wine_commands));

    return 0;
}