#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include "common.h"

#define COMMAND(GROUP, COMMAND)\
int GROUP##_##COMMAND(int argc, char** argv)


#define COMMAND_HELP(GROUP, MSG) \
    COMMAND(GROUP, help) \
    { \
        fprintf(stderr, USAGE_STR MSG " <command>\n\nList of commands:\n"); \
        print_help(GROUP##_commands, ARRAY_LEN(GROUP##_commands)); \
        return 0; \
    }

#define COMMAND_GROUP(GROUP) \
    int GROUP(int argc, char** argv) \


#define COMMAND_GROUP_FUNC(GROUP) \
    COMMAND_GROUP(GROUP) \
    { \
        if (argc > 1) \
            for (int i = 0; i < ARRAY_LEN(GROUP##_commands); ++i) \
                if (!strcmp(GROUP##_commands[i].name, argv[1])) return GROUP##_commands[i].func(argc-1, argv+1); \
        return GROUP##_help(argc-1, argv+1); \
    }

#endif