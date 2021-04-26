
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <libgen.h>
#include "lutris.h"
#include "net.h"

static const struct Command lutris_commands[] = {
    { .name = "info",    .func = lutris_info,    .description = "show information about a lutris script" },
    { .name = "install", .func = lutris_install, .description = "install a lutris script" },
    { .name = "list",    .func = lutris_list,    .description = "list installers for a game"},
    { .name = "search",  .func = lutris_search,  .description = "search for lutris installers" },
};

static const struct Flag lutris_flags[] = {
    { .name = "help", .variant = DOUBLE, .func = lutris_help, .description = "show this message"}
};

char* getpwd()
{
    char* pwd = malloc(255);
    pwd = getcwd(pwd, 255);
    return pwd;
}

COMMAND_GROUP_FUNC(lutris)

COMMAND(lutris, search)
{
    if (argc == 2 && argv[1][0] != '\0')
    {
        // argv being modifyable is not always a given so lets
        // lets make a mutable copy
        char* str = strdup(argv[1]);
        // in the case we need to do replacing we allocate more
        // since we'll free in anyway
        // the smallest strlen can return is 0 the longest
        // escapeString can return is strlen*3
        size_t allocsize = strlen(str) * 3;
        str = realloc(str, allocsize);
        lutris_escapeString(str, allocsize);
        char* url = malloc(strlen(GAME_SEARCH_API) + strlen(str));
        sprintf(url, GAME_SEARCH_API, str);

        struct json_object* queryresult = fetchJSON(url);
        free(str);
        free(url);

        if (queryresult)
        {
            struct json_object* searchresults;
            json_object_object_get_ex(queryresult, "results", &searchresults);
            if (searchresults)
            {
                struct json_object* values[2];
                size_t arrlen = json_object_array_length(searchresults);

                for (size_t i = 0; i < arrlen; ++i)
                {
                    struct json_object* entry = json_object_array_get_idx(searchresults, i);
                    json_object_object_get_ex(entry, "name", &values[0]);
                    json_object_object_get_ex(entry, "slug", &values[1]);

                    printf("%s - %s\n", json_object_get_string(values[0]), json_object_get_string(values[1]));
                }
            }

            json_object_put(queryresult);
        }
    }
    else
    {
        puts(USAGE_STR " lutris search <text>\nSearch for games and list their slug");
    }

    return 0;
}

COMMAND(lutris, list)
{
    if (argc == 2)
    {
        // argv being modifyable is not always a given so lets
        // lets make a mutable copy
        char* str = strdup(argv[1]);
        // in the case we need to do replacing we allocate more
        // since we'll free in anyway
        // the smallest strlen can return is 0 the longest
        // escapeString can return is strlen*3
        size_t allocsize = strlen(str) * 3;
        str = realloc(str, allocsize);
        lutris_escapeString(str, allocsize);
        char* url = malloc(strlen(GAME_INSTALLER_API) + strlen(str));
        sprintf(url, GAME_INSTALLER_API, str);

        struct json_object* queryresult = fetchJSON(url);
        free(str);
        free(url);

        if (queryresult)
        {
            struct json_object* installers;
            json_object_object_get_ex(queryresult, "installers", &installers);
            if (installers)
            {
                struct json_object* values[4];
                size_t arrlen = json_object_array_length(installers);

                for (size_t i = 0; i < arrlen; ++i)
                {
                    struct json_object* entry = json_object_array_get_idx(installers, i);
                    json_object_object_get_ex(entry, "slug", &values[0]);
                    json_object_object_get_ex(entry, "runner", &values[1]);
                    json_object_object_get_ex(entry, "name", &values[2]);
                    json_object_object_get_ex(entry, "version", &values[3]);

                    printf("%s - (%s) %s %s\n", json_object_get_string(values[0]), json_object_get_string(values[1]), json_object_get_string(values[2]), json_object_get_string(values[3]));
                }
            }

            json_object_put(queryresult);
        }
    }
    else
    {
        puts(USAGE_STR " lutris list <slug>\nList game installers");
    }

    return 0;
}

COMMAND(lutris, install)
{
    if (argc == 2)
    {
        struct script_t installer = lutris_getInstaller(argv[1]);

        if (installer.error == NONE)
        {
            printf("Install %s - %s to the current directory?\n\nNOTE:\tpolecat will use the wine that is on your path\n\t\tit is suggested to install the right wine version\n\t\tand use `wine env` to put it on the path\n\n(y/N)\n", installer.name, installer.version);

            int inp = getchar();
            if (inp == 'y' || inp == 'Y')
            {
                // fetch all files required by installer
                for (size_t i = 0; i < installer.filecount; ++i)
                {
                    char* filename = basename(installer.files[i]->url);
                    if (isFile(filename))
                    {
                        printf("%s found. Not Downloading\n", filename);
                    }
                    else
                    {
                        printf("Dowloading %s\n", filename);
                        downloadToFile(installer.files[i]->url, filename, 1);
                    }
                }


                for (size_t i = 0; i < installer.directivecount; ++i)
                {
                    switch(installer.directives[i]->command)
                    {

                        case MOVE:
                            // TODO
                            break;

                        case MERGE:
                            // TODO
                            break;
                        case EXTRACT:
                            // TODO
                            break;
                        case COPY:
                            // TODO
                            break;
                        case CHMODX:
                            // TODO
                            break;
                        case EXECUTE:
                            // TODO
                            break;
                        case WRITE_FILE:
                        case WRITE_JSON:
                            // TODO
                            break;

                        case WRITE_CONFIG:
                            // TODO
                            break;

                        case INPUT_MENU:
                            installer.variablecount++;
                            installer.variables = realloc(installer.variables, installer.variablecount * sizeof(void*));
                            installer.variables[installer.variablecount-1] = malloc(sizeof(struct list_t));
                            installer.variables[installer.variablecount-1]->key = installer.directives[i]->arguments[0];
                            installer.variables[installer.variablecount-1]->type = value_string;
                            installer.variables[installer.variablecount-1]->value.str = installer.directives[i]->arguments[1];
                            break;

                        case INSERT_DISC:
                            // TODO
                            break;

                        case TASK:

                            parseVar(&installer.directives[i]->arguments[0], installer.variables, installer.variablecount);
                            setenv("WINEPREFIX", installer.directives[i]->arguments[0], 1);
                            switch(installer.directives[i]->task)
                            {
                                case WINEEXEC:
                                    printf("WINEEXEC\n");
                                    break;

                                case WINETRICKS:
                                    printf("WINETRICKS\n");
                                    break;

                                case CREATE_PREFIX:
                                    printf("CREATE_PREFIX\n");
                                    setenv("WINEDEBUG", "-all", 1);
                                    system("wineboot");
                                    unsetenv("WINEDEBUG");
                                    break;

                                case SET_REGEDIT:
                                    printf("SET_REGEDIT\n");
                                    break;

                                case WINEKILL:
                                    printf("WINEKILL\n");
                                    break;

                                case TASKKEYWORDMAX:
                                case NO_TASK:
                                    break;

                                case UNKNOWN_TASK:
                                    printf("Unknown Task\n");
                                    break;
                            }
                            break;

                        case UNKNOWN_DIRECTIVE:
                            puts("Unknown directive\nIf you see this please report it with your system information and steps to reproduce this.");
                            break;

                        default:
                            UNREACHABLE
                            break;
                    }
                }
            }
        }
        else
        {
            switch(installer.error)
            {
                case NO_JSON:
                case NO_SLUG:
                    puts("No Installer with that ID was found");
                    break;

                case NO_SCRIPT:
                    puts("Installer has no script");
                    break;

                case NO_INSTALLER:
                    puts("Script has no install directives");
                    break;

                default:
                    UNREACHABLE
                    break;
            }
        }

        lutris_freeInstaller(&installer);
    }
    return 0;
}

COMMAND(lutris, info)
{
    if (argc == 2)
    {
        struct script_t installer = lutris_getInstaller(argv[1]);

        if (installer.error > NO_SLUG || installer.error == NONE)
        {

            printf("[%s]", runnerStr[installer.runner]);
            if (installer.wine) printf("[%s]", installer.wine);

            printf(" %s - %s\n", installer.name, installer.version);

            if (installer.description) puts(installer.description);
            if (installer.notes) puts(installer.notes);


            if (installer.variablecount)
            {
                puts("\nVariables:");
                for (size_t i = 0; i < installer.variablecount; ++i)
                {
                    printf("\t%s -> ", installer.variables[i]->key);
                    switch (installer.variables[i]->type)
                    {
                        case value_function:
                            {
                                char* out = installer.variables[i]->value.func();
                                puts(out);
                                free(out);
                            }
                            break;

                        case value_string:
                            puts(installer.variables[i]->value.str);
                            break;
                    }
                }
            }

            if (installer.filecount)
            {
                puts("\nFiles:");
                for (size_t i = 0; i < installer.filecount; ++i)
                {
                    printf("\t%s -> %s\n", installer.files[i]->name, installer.files[i]->url);
                }
            }

            if (installer.directivecount)
            {

                puts("\nDirectives:");
                for (size_t i = 0; i < installer.directivecount; ++i)
                {
                    printf("\t%s", keywordstr[installer.directives[i]->command]);

                    if (installer.directives[i]->task != NO_TASK) printf(" %s", taskKeywordstr[installer.directives[i]->task]);

                    for (size_t j = 0; j < installer.directives[i]->size; ++j)
                    {
                        printf(" %s", installer.directives[i]->arguments[j]);
                    }

                    puts("");
                }
            }
        }
        else
        {
            printf("Couldn't find an installer by the name '%s'\n", argv[1]);
        }

        lutris_freeInstaller(&installer);
    }
    else
    {
        puts(USAGE_STR " lutris info <installer-name>\nThe Name of an installer can be found by viewing the install script of an installer");
    }
    return 0;
}

COMMAND_HELP(lutris, " lutris")

void lutris_escapeString(char* str, size_t size)
{
    char* tail = str + size;

    while (str != tail)
    {
        switch (*str)
        {
            // from rfc3986 https://tools.ietf.org/html/rfc3986#page-13
            // we generally don't want delimiters
            // so we strip them if found
            case ':':
            case '/':
            case '?':
            case '#':
            case '[':
            case ']':
            case '@':
            case '!':
            case '$':
            case '&':
            case '\'':
            case '(':
            case ')':
            case '*':
            case '+':
            case ',':
            case ';':
            case '=':
                // since dest and src overlap we need a non overlapping solution
                memmove(str, str+1, (size_t)(tail-str-1));
                break;

            case ' ':
                // spaces are special and need to be encoded
                memmove(str+3, str+1, (size_t)(tail-str-3));
                str[0] = '%';
                str[1] = '2';
                str[2] = '0';
                str += 3;
                if (str >= tail) str[3] = '\0';
                break;

            /* fallthrough */
            default:
                ++str;
        }
    }
}

struct script_t lutris_getInstaller(char* installername)
{
    struct script_t installer;
    installer.name = NULL;
    installer.version = NULL;
    installer.runner = UNKNOWN_RUNNER;
    installer.description = NULL;
    installer.notes = NULL;
    installer.wine = NULL;
    installer.directives = NULL;
    installer.directivecount = 0;
    installer.files = NULL;
    installer.filecount = 0;
    installer.error = NONE;
    installer.variables = NULL;
    installer.variablecount = 0;

    if (installername)
    {
        char installerurl[PATH_MAX];
        strncpy(installerurl, INSTALLER_API, sizeof(installerurl));
        strncat(installerurl, installername, sizeof(installerurl) - strlen(installerurl) - 1);

        struct json_object* installerjson = fetchJSON(installerurl);

        if (installerjson)
        {
            struct json_object* count, *results, *slug;

            json_object_object_get_ex(installerjson, "count", &count);
            json_object_object_get_ex(installerjson, "results", &results);
            slug = json_object_array_get_idx(results, 0);

            if (json_object_get_int(count) == 1)
            {
                struct json_object* script, *scriptinstall, *files;

                if(json_object_object_get_ex(slug, "script", &script))
                {
                    {
                        struct json_object* name, *version, *runner, *description, *notes, *wine, *winever;
                        const char* namestr, *versionstr, *runnerstr, *descriptionstr, *notesstr, *winestr;

                        json_object_object_get_ex(slug, "name", &name);
                        namestr = json_object_get_string(name);
                        installer.name = malloc( strlen(namestr) * sizeof(char) +1 );
                        strcpy(installer.name, namestr);

                        json_object_object_get_ex(slug, "version", &version);
                        versionstr = json_object_get_string(version);
                        installer.version = malloc( strlen(versionstr) * sizeof(char) +1 );
                        strcpy(installer.version, versionstr);

                        json_object_object_get_ex(slug, "runner", &runner);
                        runnerstr = json_object_get_string(runner);
                        for (int i = 0; i < RUNNERMAX; ++i)
                        {
                            if(!strcmp(runnerstr, runnerStr[i]))
                            {
                                installer.runner = (enum runner_t)i;
                                break;
                            }
                        }

                        json_object_object_get_ex(slug, "description", &description);
                        if (description)
                        {
                            descriptionstr = json_object_get_string(description);
                            installer.description = malloc( strlen(descriptionstr) * sizeof(char) +1 );
                            strcpy(installer.description, descriptionstr);
                        }

                        json_object_object_get_ex(slug, "notes", &notes);
                        if (notes)
                        {
                            notesstr = json_object_get_string(notes);
                            installer.notes = malloc( strlen(notesstr) * sizeof(char) +1 );
                            strcpy(installer.notes, notesstr);
                        }

                        json_object_object_get_ex(script, "wine", &wine);
                        json_object_object_get_ex(wine, "version", &winever);
                        if (winever)
                        {
                            winestr = json_object_get_string(winever);
                            installer.wine = malloc( strlen(winestr) * sizeof(char) +1 );
                            strcpy(installer.wine, winestr);
                        }

                    }

                    if (json_object_object_get_ex(script, "files", &files))
                    {
                        installer.filecount = json_object_array_length(files);
                        // we need to assign variables for all files and for the GAMEDIR
                        installer.variablecount = installer.filecount + 1;
 
                        installer.files = malloc(installer.filecount * sizeof(void*));
                        installer.variables = malloc(installer.variablecount * sizeof(void*));
                        for (size_t i = 0; i < installer.filecount; ++i)
                        {
                            struct json_object* file = json_object_array_get_idx(files, i);
                            struct lh_entry* entry = json_object_get_object(file)->head;

                            installer.files[i] = malloc(sizeof(struct file_t));

                            {
                                size_t namelen = strlen((char*)entry->k);
                                installer.files[i]->name = malloc(namelen * sizeof(char) +1);
                                strcpy(installer.files[i]->name, (char*)entry->k);
                            }

                            const char* urlstr;

                            if(json_object_get_type((struct json_object*)entry->v) == json_type_object)

                            {
                                struct json_object* url;
                                json_object_object_get_ex((struct json_object*)entry->v, "url", &url);
                                urlstr = json_object_get_string(url);
                            }
                            else
                            {
                                urlstr = json_object_get_string((struct json_object*)entry->v);
                            }

                            {
                                size_t urllen = strlen(urlstr);
                                installer.files[i]->url = malloc(urllen * sizeof(char) +1);
                                strcpy(installer.files[i]->url, urlstr);
                            }

                            installer.variables[i] = malloc(sizeof(struct list_t));
                            installer.variables[i]->key = installer.files[i]->name;
                            installer.variables[i]->type = value_string;
                            installer.variables[i]->value.str = basename(installer.files[i]->url);
                        }

                        installer.variables[installer.variablecount-1] = malloc(sizeof(struct list_t));
                        installer.variables[installer.variablecount-1]->key = "GAMEDIR";
                        installer.variables[installer.variablecount-1]->type = value_function;
                        installer.variables[installer.variablecount-1]->value.func = getpwd;
                    }

                    if (json_object_object_get_ex(script, "installer", &scriptinstall))
                    {
                        installer.directivecount = json_object_array_length(scriptinstall);

                        installer.directives = malloc(installer.directivecount * sizeof(void*));
                        for (size_t i = 0; i < installer.directivecount; ++i)
                        {
                            struct json_object* step = json_object_array_get_idx(scriptinstall, i);
                            struct json_object* directive;

                            installer.directives[i] = malloc(sizeof(struct directive_t));
                            installer.directives[i]->size = 0;
                            installer.directives[i]->command = UNKNOWN_DIRECTIVE;
                            installer.directives[i]->task = NO_TASK;

                            for (int l = 0; l < KEYWORDMAX; ++l)
                            {
                                if (json_object_object_get_ex(step, keywordstr[l], &directive))
                                {
                                    struct json_object* options[5];
                                    switch (l)
                                    {
                                        case MOVE:
                                        case COPY:
                                        case MERGE:
                                            json_object_object_get_ex(directive, "src", &options[0]);
                                            json_object_object_get_ex(directive, "dst", &options[1]);
                                            installer.directives[i]->size = 2;
                                            break;

                                        case EXTRACT:
                                            json_object_object_get_ex(directive, "file", &options[0]);
                                            installer.directives[i]->size = 1;
                                            break;

                                        case CHMODX:
                                            options[0] = directive;
                                            installer.directives[i]->size = 1;
                                            break;

                                        case EXECUTE:
                                            if(!json_object_object_get_ex(directive, "command", &options[0])) json_object_object_get_ex(directive, "file", &options[0]);
                                            installer.directives[i]->size = 1;
                                            break;

                                        case WRITE_FILE:
                                            json_object_object_get_ex(directive, "file", &options[0]);
                                            json_object_object_get_ex(directive, "content", &options[1]);
                                            installer.directives[i]->size = 2;
                                            break;

                                        case WRITE_CONFIG:
                                            json_object_object_get_ex(directive, "file", &options[0]);
                                            json_object_object_get_ex(directive, "section", &options[1]);
                                            json_object_object_get_ex(directive, "key", &options[2]);
                                            json_object_object_get_ex(directive, "value", &options[3]);
                                            installer.directives[i]->size = 4;
                                            break;

                                        case WRITE_JSON:
                                            json_object_object_get_ex(directive, "file", &options[0]);
                                            json_object_object_get_ex(directive, "data", &options[1]);
                                            installer.directives[i]->size = 2;
                                            break;

                                        case INPUT_MENU:
                                            json_object_object_get_ex(directive, "id", &options[0]);
                                            json_object_object_get_ex(directive, "preselect", &options[1]);
                                            json_object_object_get_ex(directive, "description", &options[2]);
                                            installer.directives[i]->size = 3;
                                            break;

                                        case INSERT_DISC:
                                            json_object_object_get_ex(directive, "requires", &options[0]);
                                            installer.directives[i]->size = 1;
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
                                                            json_object_object_get_ex(directive, "prefix", &options[1]);
                                                            json_object_object_get_ex(directive, "executable", &options[2]);
                                                            installer.directives[i]->size = 2;
                                                            break;

                                                        case WINETRICKS:
                                                            json_object_object_get_ex(directive, "prefix", &options[1]);
                                                            json_object_object_get_ex(directive, "app", &options[2]);
                                                            installer.directives[i]->size = 2;
                                                            break;

                                                        case CREATE_PREFIX:
                                                        case WINEKILL:
                                                            json_object_object_get_ex(directive, "prefix", &options[1]);
                                                            installer.directives[i]->size = 1;
                                                            break;

                                                        case SET_REGEDIT:
                                                            json_object_object_get_ex(directive, "prefix", &options[1]);
                                                            json_object_object_get_ex(directive, "path", &options[2]);
                                                            json_object_object_get_ex(directive, "key", &options[3]);
                                                            json_object_object_get_ex(directive, "value", &options[4]);
                                                            installer.directives[i]->size = 4;
                                                            break;
                                                    }
                                                    installer.directives[i]->task = (enum task_t)k;
                                                    break;
                                                }
                                            }
                                            break;
                                    }
                                    installer.directives[i]->command = (enum keyword_t)l;

                                    const char* str;
                                    uint8_t offset = 0;
                                    if (installer.directives[i]->task != NO_TASK)
                                    {
                                        offset = 1;
                                    }

                                    installer.directives[i]->arguments = malloc(installer.directives[i]->size * sizeof(char*));
                                    for (size_t j = 0; j < installer.directives[i]->size; ++j)
                                    {
                                        str = json_object_get_string(options[j+offset]);
                                        // there will always be some attributes ommited
                                        // from an installer, just replace those with an empty string
                                        if (!str) str = "";
                                        installer.directives[i]->arguments[j] = malloc(strlen(str) * sizeof(char) +1);
                                        strcpy(installer.directives[i]->arguments[j], str);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    else installer.error = NO_INSTALLER;
                }
                else installer.error = NO_SCRIPT;
            }
            else installer.error = NO_SLUG;

            json_object_put(installerjson);
        }
        else installer.error = NO_JSON;
    }

    return installer;
}

void lutris_freeInstaller(struct script_t* installer)
{
    if (installer)
    {
        free(installer->name);
        free(installer->version);
        free(installer->description);
        free(installer->notes);
        free(installer->wine);

        if (installer->directives)
        {
            for (size_t i = 0; i < installer->directivecount; ++i)
            {
                for (size_t j = 0; j < installer->directives[i]->size; ++j)
                {
                    free(installer->directives[i]->arguments[j]);
                }

                free(installer->directives[i]->arguments);
                free(installer->directives[i]);
            }
            free(installer->directives);
        }

        if (installer->files)
        {
            for (size_t i = 0; i < installer->filecount; ++i)
            {
                free(installer->files[i]->name);
                free(installer->files[i]->url);
                free(installer->files[i]);
            }
            free(installer->files);
        }

        if (installer->variables)
        {
            for (size_t i = 0; i < installer->variablecount; ++i)
            {
                free(installer->variables[i]);
            }
            free(installer->variables);
        }
    }
}

size_t parseVar(char** pvar, struct list_t** variables, size_t variable_count)
{
    char* var = *pvar;
    char* head = var;
    char* tail, *end;
    char* buf, *value;

    size_t offset;
    size_t keylen, backlen;

    size_t varcount = 0;


    while (*head != '\0')
    {
        if (*head == VARIABLESIGN)
        {
            // ensure sanity by clearing variables
            buf = NULL;
            value = NULL;

            end = strlen(var) + var;

            offset = (size_t)(head-var);

            // find variable key 
            tail = ++head;
            while (*tail <= 'z' && *tail >= 'A') // we ONLY accept ascii variables
            {
                ++tail;
            }

            // setup variables for reuse
            keylen = (size_t)(tail-head);
            backlen = (size_t)(end-tail);

            if (keylen > 0)
            {
                // resolve variable key and store it
                buf = malloc(keylen+1);
                strncpy(buf, head, keylen+1);
                buf[keylen] = '\0';
                for (size_t i = 0; i < variable_count; ++i)
                {
                    if (strncmp(variables[i]->key, buf, keylen+1) == 0)
                    {
                        switch (variables[i]->type)
                        {
                            case value_string:
                                value = strdup(variables[i]->value.str);
                                break;

                            case value_function:
                                value = variables[i]->value.func();
                                break;

                            default:
                                UNREACHABLE
                                break;
                        }
                    }
                }
                free(buf);
            }

            if (!value) continue;

            // copy everything from variable key end to end of string
            buf = malloc(backlen+1);
            strncpy(buf, tail, backlen+1);

            size_t varsize = offset + strlen(value) + backlen + 1;

            var = realloc(var, varsize);
            // end of the string up until the variable
            // we cannot fetch this after the realloc because it points to completly different memory making pointer math impossible
            var[offset] = '\0';
            strncat(var, value,  varsize - strlen(var) - 1);
            strncat(var, buf, varsize - strlen(var) - 1);   
            // cleanup
            free(value);
            free(buf);

            varcount++;

            // bring your head back to the world of living memory
            head = var + offset;
        }

        head++;
    }

    // point pvar back to the string
    *pvar = var;

    return varcount;
}
