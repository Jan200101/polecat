#ifndef LUTRIS_H
#define LUTRIS_H

#include <json.h>

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
    WINE,
    LINUX,

    RUNNERMAX,
    UNKNOWN_RUNNER
};

static const char runnerStr[RUNNERMAX][0xF] = 
{
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

struct script {
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
    enum errors error;
};

int lutris(int, char**);
int lutris_install(int, char**);
int lutris_info(int, char**);
int lutris_help(int, char**);

void lutris_getInstallerURL(char*, char*, size_t);
struct script lutris_getInstaller(char*);
void lutris_freeInstaller(struct script*);

#endif