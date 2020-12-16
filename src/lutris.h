#ifndef LUTRIS_H
#define LUTRIS_H

#include <stddef.h>
#include <json.h>

#define VARIABLESIGN '$'

enum keyword {
    MOVE = 0,
    MERGE,
    EXTRACT,
    COPY,
    CHMODX,
    EXECUTE,
    WRITE_FILE,
    WRITE_CONFIG,
    WRITE_JSON,
    INPUT_MENU,
    INSERT_DISC,
    TASK,

    KEYWORDMAX,
    UNKNOWN_DIRECTIVE
};

static const char keywordstr[KEYWORDMAX][0xF] = {
    "move",
    "merge",
    "extract",
    "copy",
    "chmodx",
    "execute",
    "write_file",
    "write_config",
    "write_json",
    "input_menu",
    "insert-disc",
    "task",
};

enum task {
    WINEEXEC = 0,
    WINETRICKS,
    CREATE_PREFIX,
    SET_REGEDIT,
    WINEKILL,

    TASKKEYWORDMAX,
    NO_TASK,
    UNKNOWN_TASK
};

static const char taskKeywordstr[TASKKEYWORDMAX][0xF] = 
{
    "wineexec",
    "winetricks",
    "create_prefix",
    "set_regedit",
    "winekill",
};

enum runner_t {
    UNKNOWN_RUNNER,
    WINE,
    LINUX,

    RUNNERMAX
};

/*
 * a list of all available runners could be fetched from lutris
 * but we keep a local copy of all supported runners
 * to reduce the amount of API calls needed
 */
static const char runnerStr[RUNNERMAX][0xF] = 
{
    "unknown",
    "wine",
    "linux",
};


enum errors {
    NONE,
    NO_JSON,
    NO_SLUG,
    NO_SCRIPT,
    NO_INSTALLER,
};

struct directive_t {
    enum keyword command;
    enum task task;
    char** arguments;
    size_t size;
};

struct file_t {
    char* filename;
    char* url;
};

enum value_type_t {
    value_string,
    value_function
};

struct list_t {
    char* key;
    enum value_type_t type;
    union
    {
        char* str;
        char* (*func)();
    } value;
};

// my best attempt at representing a Lutris installer as a struct
struct script_t {
    char* name;
    char* version;
    enum runner_t runner;
    char* description;
    char* notes;
    char* wine;

    struct directive_t** directives;
    size_t directivecount;

    struct file_t** files;
    size_t filecount;

    struct list_t** variables;
    size_t variablecount;

    enum errors error;
};

#include "command.h"

COMMAND(lutris, debug);

COMMAND_GROUP(lutris);
COMMAND(lutris, install);
COMMAND(lutris, info);
COMMAND(lutris, help);

void lutris_getInstallerURL(char*, char*, size_t);
struct script_t lutris_getInstaller(char*);
void lutris_freeInstaller(struct script_t*);

size_t parseVar(char**, struct list_t*, size_t);

#endif
