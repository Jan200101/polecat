#include <stdio.h>
#include <string.h>
#include <linux/limits.h>

#include "lutris.h"
#include "net.h"
#include "common.h"

const static struct Command lutris_commands[] = {
    { .name = "install", .func = lutris_install, .description = "install a lutris script" },
    { .name = "info",    .func = lutris_info,    .description = "show information about a lutris script" },
};

int lutris(int argc, char** argv)
{
    if (argc > 1)
    {
        for (int i = 0; i < ARRAY_LEN(lutris_commands); ++i)
        {
            if (!strcmp(lutris_commands[i].name, argv[1])) return lutris_commands[i].func(argc-1, argv+1);
        }
    } 


    return lutris_help(argc, argv);
}


int lutris_install(int argc, char** argv)
{

}

int lutris_info(int argc, char** argv)
{
    if (argc == 2)
    {
        char installerurl[PATH_MAX];
        lutris_getInstallerURL(installerurl, argv[1]);

        struct json_object* installer = fetchJSON(installerurl);

        if (installer)
        {
            struct json_object* count, *results, *slug;

            json_object_object_get_ex(installer, "count", &count);
            json_object_object_get_ex(installer, "results", &results);
            slug = json_object_array_get_idx(results, 0);

            if (json_object_get_int(count) != 0)
            {
                struct json_object* name, *version, *runner, *description, *notes, *script, *scriptinstall, *wine, *winever;
                json_object_object_get_ex(slug, "name", &name);
                json_object_object_get_ex(slug, "version", &version);
                json_object_object_get_ex(slug, "runner", &runner);
                json_object_object_get_ex(slug, "description", &description);
                json_object_object_get_ex(slug, "notes", &notes);
                json_object_object_get_ex(slug, "script", &script);
                json_object_object_get_ex(script, "installer", &scriptinstall);

                json_object_object_get_ex(script, "wine", &wine);
                json_object_object_get_ex(wine, "version", &winever);

                printf("[%s]", json_object_get_string(runner));
                if (winever) printf("[%s]", json_object_get_string(winever));

                printf(" %s - %s\n", json_object_get_string(name), json_object_get_string(version));

                if (description) printf("\n%s\n", json_object_get_string(description));
                if (notes) printf("\n%s\n", json_object_get_string(notes));

                if (scriptinstall)
                {
                    puts("\ninstall script:");
                    for (int i = 0; i < json_object_array_length(scriptinstall); ++i)
                    {
                        struct json_object* step = json_object_array_get_idx(scriptinstall, i);
                        struct json_object* directive;

                        for (int l = 0; l < KEYWORDMAX; ++l)
                        {
                            json_object_object_get_ex(step, keywordstr[l], &directive);
                            if (directive)
                            {
                                struct json_object* options[5];
                                printf("  - ");
                                switch (l)
                                {
                                    case MOVE:
                                    case COPY:
                                    case MERGE:
                                        json_object_object_get_ex(directive, "src", &options[0]);
                                        json_object_object_get_ex(directive, "dst", &options[1]);                                    
                                        printf("%s %s %s\n", keywordstr[l], json_object_get_string(options[0]), json_object_get_string(options[1]));
                                        break;

                                    case EXTRACT:
                                        json_object_object_get_ex(directive, "file", &options[0]);
                                        printf("%s %s\n", keywordstr[l], json_object_get_string(options[0]));
                                        break;

                                    case CHMODX:
                                        printf("%s %s\n", keywordstr[l], json_object_get_string(directive));
                                        break;

                                    case EXECUTE:
                                        json_object_object_get_ex(directive, "file", &options[0]);
                                        printf("%s %s\n", keywordstr[l], json_object_get_string(options[0]));
                                        break;

                                    case WRITE_FILE:
                                        json_object_object_get_ex(directive, "file", &options[0]);
                                        printf("%s %s\n", keywordstr[l], json_object_get_string(options[0]));
                                        break;

                                    case WRITE_CONFIG:
                                        json_object_object_get_ex(directive, "file", &options[0]);
                                        json_object_object_get_ex(directive, "section", &options[1]);
                                        json_object_object_get_ex(directive, "key", &options[2]);
                                        json_object_object_get_ex(directive, "value", &options[3]);

                                        printf("%s %s [%s] %s = %s\n", keywordstr[l], json_object_get_string(options[0]), json_object_get_string(options[1]), json_object_get_string(options[2]), json_object_get_string(options[3]));
                                        break;

                                    case WRITE_JSON:
                                        printf("%s\n", keywordstr[l]);
                                        break;

                                    case INPUT_MENU:
                                        json_object_object_get_ex(directive, "description", &options[0]);
                                        printf("%s `%s'\n", keywordstr[l], json_object_get_string(options[0]));
                                        break;

                                    case INSERT_DISC:
                                        json_object_object_get_ex(directive, "requires", &options[0]);
                                        printf("%s %s\n", keywordstr[l], json_object_get_string(options[0]));
                                        break;

                                    case TASK:
                                        json_object_object_get_ex(directive, "name", &options[0]);
                                        const char* name = json_object_get_string(options[0]);
                                        for (int k = 0; k <= TASKKEYWORDMAX; ++k)
                                        {
                                            if (!strcmp(name, taskKeywordstr[k]))
                                            {
                                                switch(k)
                                                {
                                                    case WINEEXEC:
                                                        json_object_object_get_ex(directive, "executable", &options[1]);
                                                        printf("%s %s\n", name, json_object_get_string(options[1]));
                                                        break;

                                                    case WINETRICKS:
                                                        json_object_object_get_ex(directive, "app", &options[1]);
                                                        printf("%s %s\n", name, json_object_get_string(options[1]));
                                                        break;

                                                    case CREATE_PREFIX:
                                                    case WINEKILL:
                                                        json_object_object_get_ex(directive, "prefix", &options[1]);
                                                        printf("%s %s\n", name, json_object_get_string(options[1]));
                                                        break;

                                                    case SET_REGEDIT:
                                                        json_object_object_get_ex(directive, "path", &options[1]);
                                                        json_object_object_get_ex(directive, "key", &options[2]);
                                                        json_object_object_get_ex(directive, "value", &options[3]);

                                                        printf("%s %s\\%s = %s\n", name, json_object_get_string(options[1]), json_object_get_string(options[2]), json_object_get_string(options[3]));
                                                        break;

                                                    default:
                                                        puts(name);
                                                        break;
                                                }
                                                break;
                                            }
                                            if (k == TASKKEYWORDMAX) printf("FIXME: unknown task %s\n", name);
                                        }
                                        break;

                                    default:
                                        printf("FIXME: unknown %s\n", keywordstr[l]);
                                }
                                break;
                            }
                        }
                    }
                }
                else puts("no install script found");
            }
            else
            {
                printf("`%s' does not exist on lutris\n", argv[1]);
            }

            json_object_put(installer);
        }
    }
    else puts(USAGE_STR " lutris info <installer-id>\nInstaller IDs are obtained from the lutris website");

    return 0;
}

void lutris_getInstallerURL(char* buffer, char* name)
{
        strcpy(buffer, INSTALLER_API);
        strcat(buffer, name);
}
int lutris_help(int argc, char** argv)
{
    puts(USAGE_STR " lutris <command>\n\nList of commands:");

    print_help(lutris_commands, ARRAY_LEN(lutris_commands));

    return 0;
}