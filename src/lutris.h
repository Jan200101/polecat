#ifndef LUTRIS_H
#define LUTRIS_H

#include <stddef.h>
#include <json.h>

#define VARIABLESIGN '$'

#include "lutris/pre.h"
#include "lutris/enum.h"
enum keyword_t {
    #include "lutris/keyword.h"

    KEYWORDMAX,
    unknown_keyword
};

enum task_t {
    #include "lutris/task.h"

    TASKKEYWORDMAX,
    no_task,
    unknown_task
};

enum runner_t {
    #include "lutris/runner.h"

    RUNNERMAX
};
#include "lutris/array.h"
static const char keywordstr[KEYWORDMAX][0xF] = {
    #include "lutris/keyword.h"
};


static const char taskKeywordstr[TASKKEYWORDMAX][0xF] = 
{
    #include "lutris/task.h"
};

static const char runnerStr[RUNNERMAX][0xF] = 
{
    #include "lutris/runner.h"
};
#include "lutris/post.h"

enum errors {
    NONE,
    NO_JSON,
    NO_SLUG,
    NO_SCRIPT,
    NO_INSTALLER,
};

struct directive_t {
    enum keyword_t command;
    enum task_t task;
    char** arguments;
    size_t size;
};

struct file_t {
    char* name;
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
        char* (*func)(void);
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

COMMAND_GROUP(lutris);
COMMAND(lutris, search);
COMMAND(lutris, list);
COMMAND(lutris, install);
COMMAND(lutris, info);
COMMAND(lutris, help);

void lutris_escapeString(char*, size_t);
struct script_t lutris_getInstaller(char*);
void lutris_freeInstaller(struct script_t*);

size_t parseVar(char**, struct list_t**, size_t);

#endif
