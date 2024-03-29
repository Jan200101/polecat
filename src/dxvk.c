#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <json.h>

#include "dxvk.h"
#include "net.h"
#include "tar.h"
#include "config.h"

static const struct Command dxvk_commands[] = {
    { .name = "download",       .func = dxvk_download,   .description = "download and install a dxvk version" },
    { .name = "install",        .func = dxvk_install,    .description = "run the DXVK installer" },
    { .name = "list",           .func = dxvk_list,       .description = "list available dxvk versions" },
    { .name = "installed",      .func = dxvk_installed,  .description = "list installed dxvk versions" },
    { .name = "remove",         .func = dxvk_remove,     .description = "remove a dxvk version" },
};

static const struct Flag dxvk_flags[] = {
    { .name = "help",       .variant = TWO, .returns = 1, .func = dxvk_help,    .description = "show this message"},
    { .name = "no-net",     .variant = TWO, .returns = 0, .func = set_no_net,   .description = "run commands without commitment"}
};

COMMAND_GROUP_FUNC(dxvk)

COMMAND(dxvk, download)
{
    if (argc == 2)
    {
        struct json_object* runner = fetchJSON(DXVK_API);

        if (runner)
        {

            struct json_object* value, *temp, *temp2;
            uint8_t found = 0;

            char* choice = argv[1];

            for (JSON_LENGTH_TYPE i = 0; i < json_object_array_length(runner); ++i)
            {
                value = json_object_array_get_idx(runner, i);
                json_object_object_get_ex(value, "tag_name", &temp);
                if (strcmp(json_object_get_string(temp), choice) == 0)
                {
                    found = 1;
                    break;
                }
            }

            if (found)
            {
                json_object_object_get_ex(value, "assets", &temp);
                temp2 = json_object_array_get_idx(temp, 0);

                json_object_object_get_ex(temp2, "browser_download_url", &temp);

                char* name = basename((char*)json_object_get_string(temp));

                char dxvkdir[PATH_MAX];
                struct MemoryStruct* archive;

                getDXVKDir(dxvkdir, sizeof(dxvkdir));
                makeDir(dxvkdir);
                
                fprintf(stderr, "Downloading %s...\n", name);

                archive = downloadToRam(json_object_get_string(temp), 1);
                if (archive)
                {
                    fprintf(stderr, "Extracting %s\n", name);
                    extract(archive, dxvkdir);
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

            json_object_put(runner);
        }
    }
    else
    {
        fprintf(stderr, USAGE_STR " dxvk download <version>\n\nversions are obtained via '" NAME " dxvk list'\n");
    }
    return EXIT_SUCCESS;
}

COMMAND(dxvk, remove)
{
    if (argc == 2)
    {
        char* dxvkver = argv[1];

        char dxvkpath[PATH_MAX];
        getDXVKDir(dxvkpath, sizeof(dxvkpath));

        strncat(dxvkpath, "/", sizeof(dxvkpath) - strlen(dxvkpath) - 1);
        strncat(dxvkpath, dxvkver, sizeof(dxvkpath) - strlen(dxvkpath) - 1);


        if (!isDir(dxvkpath))
        {
            fprintf(stderr, "'%s' is not a downloaded DXVK version\n", dxvkver);
            return EXIT_SUCCESS;
        }

        int retval = removeDir(dxvkpath);
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

    fprintf(stderr, USAGE_STR " dxvk remove <version>\n\nInstalled dxvk versions can be obtained by using '" NAME " dxvk installed'\n");

    return EXIT_SUCCESS;
}

COMMAND(dxvk, list)
{
    struct json_object* runner = fetchJSON(DXVK_API);

    if (runner)
    {
        int istty = isatty(STDOUT_FILENO); 
        if (istty) puts("Available DXVK versions:");

        for (JSON_LENGTH_TYPE i = 0; i < json_object_array_length(runner); ++i)
        {
            struct json_object* version = json_object_array_get_idx(runner, i);
            struct json_object* name;

            json_object_object_get_ex(version, "tag_name", &name);
            if (istty) printf(" - ");
            printf("%s\n", json_object_get_string(name));
        }

        json_object_put(runner);
    }

    return EXIT_SUCCESS;
}

COMMAND(dxvk, install)
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
            fprintf(stderr, "'%s' is not an downloaded DXVK version. Did you mean '" NAME " dxvk download'?\n", dxvkver);
            return EXIT_SUCCESS;
        }

        strncat(dxvkpath, DXVKSETUP, sizeof(dxvkpath) - strlen(dxvkpath) - 1);

        if (isFile(dxvkpath))
        {
            // quote the dxvkpath so whitespace won't cause trouble
            {
                size_t size = strlen(dxvkpath) + 1;
                if (size > sizeof(dxvkpath)) size = sizeof(dxvkpath);

                memmove(dxvkpath+1, dxvkpath, size - 1);
                dxvkpath[0] = '"';

                strncat(dxvkpath, "\"", sizeof(dxvkpath) - size);

            }
            strncat(dxvkpath, " install", sizeof(dxvkpath) - strlen(dxvkpath) - 1);

            return system(dxvkpath);
        }
        else
        {
            fprintf(stderr, "cannot find the setup script for '%s'\n", dxvkver);
        }

    }
    else
    {
        fprintf(stderr, "Specify a what DXVK version to install.\nUse '" NAME " dxvk installed' to list available versions\n");
    }

        
    return EXIT_SUCCESS;
}

COMMAND(dxvk, installed)
{
    char dxvkdir[PATH_MAX];
    getDXVKDir(dxvkdir, sizeof(dxvkdir));

    size_t dxvklen = strlen(dxvkdir);
    dxvkdir[dxvklen] = '/';

    DIR *dir;
    struct dirent *ent;

    int intty = isatty(STDOUT_FILENO);

    if (intty) puts("Downloaded DXVK versions:");
    if ((dir = opendir(dxvkdir)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] == '.') continue;
            strncat(dxvkdir, ent->d_name, sizeof(dxvkdir) - dxvklen - 1);
            int isdirec = isDir(dxvkdir);
            dxvkdir[dxvklen+1] = '\0';

            if (!isdirec) continue;

            if (intty) printf(" - ");
            printf("%s\n", ent->d_name);
        }
        closedir(dir);
    } 

    return EXIT_SUCCESS;
}

COMMAND_HELP(dxvk, " dxvk")
