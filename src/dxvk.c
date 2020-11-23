
#include <stdio.h>
#include <string.h>
#include <json.h>
#include <libgen.h>
#include <linux/limits.h>
#include <dirent.h>

#include "dxvk.h"
#include "net.h"
#include "tar.h"
#include "common.h"
#include "config.h"

const static struct Command dxvk_commands[] = {
    { .name = "download",       .func = dxvk_download,   .description = "download and install a dxvk version" },
    { .name = "list",           .func = dxvk_list,       .description = "list available dxvk versions" },
    { .name = "install",        .func = dxvk_install,    .description = "run the DXVK installer" },
    { .name = "list-installed", .func = dxvk_installed,  .description = "list installed dxvk versions" },
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

    return dxvk_help(argc-1, argv+1);
}


int dxvk_download(int argc, char** argv)
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
                struct MemoryStruct* archive;

                char dxvkdir[PATH_MAX];
                getDXVKDir(dxvkdir, sizeof(dxvkdir));
                makeDir(dxvkdir);
                
                printf("Downloading %s\n", name);

                archive = downloadToRam(json_object_get_string(assets));
                if (archive)
                {
                    printf("Extracting %s\n", name);
                    extract(archive, dxvkdir);
                }
                else
                {
                    puts("Something went wrong. The archive went missing");
                }

                free(archive->memory);
                free(archive);
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

int dxvk_install(int argc, char** argv)
{
    if (argc > 1)
    {
        char dxvkpath[PATH_MAX];
        getDXVKDir(dxvkpath, sizeof(dxvkpath));
        char* dxvkver = argv[1];

        strncat(dxvkpath, "/", sizeof(dxvkpath) - strlen(dxvkpath) - 1);
        strncat(dxvkpath, dxvkver, sizeof(dxvkpath) - strlen(dxvkpath) - 1);

        if (!isDir(dxvkpath))
        {
            printf("`%s' is not an downloaded DXVK version\n", dxvkver);
            return 0;
        }

        strncat(dxvkpath, DXVKSETUP, sizeof(dxvkpath) - strlen(dxvkpath) - 1);

        if (isFile(dxvkpath))
        {
            strncat(dxvkpath, " install", sizeof(dxvkpath) - strlen(dxvkpath) - 1);

            return system(dxvkpath);
        }
        else
        {
            printf("cannot find the setup script for `%s'\n", dxvkver);
        }

    }
    else
    {
        printf("Specify a what DXVK version to install.\nUse `" NAME " dxvk list-installed' to list available versions\n");
    }

        
    return 0;
}

int dxvk_installed(int argc, char** argv)
{
    char dxvkdir[PATH_MAX];
    getDXVKDir(dxvkdir, sizeof(dxvkdir));

    DIR *dir;
    struct dirent *ent;

    printf("Installed DXVK versions:\n");
    if ((dir = opendir(dxvkdir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
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