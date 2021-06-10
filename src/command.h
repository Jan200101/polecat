#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include "common.h"

/*
 * the current command system polecat uses is simple and depends on
 * int (*)(int, char**) function pointers, exactly the same as main
 * because keeping track of all of those is rather tedious making 
 * macros was an easier solution to get rid of the redundancy
 * for the sake of less readability.
 * argc and argv are marked as UNUSED (defined in common.h)
 * because the functions defined using this macro might not
 * actually use either of them so this is required to
 * supress warnings
 */
#define COMMAND(GROUP, COMMAND) \
    int GROUP##_##COMMAND(UNUSED int argc, UNUSED char** argv)


/*
 * all help functions in polecat look the same invoking the print_help
 * helper function defined in common.c with the command groups list
 * of available commands.
 * This also exists to reduce redundancy. 
 */
#define COMMAND_HELP(GROUP, MSG) \
    COMMAND(GROUP, help) \
    { \
        fprintf(stderr, USAGE_STR MSG " <command>\n"); \
        print_help(GROUP##_commands, ARRAY_LEN(GROUP##_commands), GROUP##_flags, ARRAY_LEN(GROUP##_flags)); \
        return 0; \
    }

/*
 * This is the same as the COMMAND macro except dedicated to the actual
 * command group name, which is the thing called from the actual main
 * function. 
 */
#define COMMAND_GROUP(GROUP) \
    int GROUP(int argc, char** argv)

#define COMMAND_GROUP_BODY_COMMANDS(GROUP) \
    for (unsigned long i = 0; i < ARRAY_LEN(GROUP##_commands); ++i) \
        if (!strcmp(GROUP##_commands[i].name, argv[1])) return GROUP##_commands[i].func(argc-1, argv+1);

#define COMMAND_GROUP_BODY_FLAGS(GROUP) \
    uint8_t found; \
    \
    for (int j = 1; j < argc; ++j) \
    { \
        found = 0; \
        if (argv[j][0] != '-') continue; \
        if(!strcmp(argv[0], "wine") && !strcmp(argv[1], "run")) break;\
        \
        for (unsigned long i = 0; i < ARRAY_LEN(GROUP##_flags); ++i) \
        { \
            if ((GROUP##_flags[i].variant & ONE && argv[j][1] == GROUP##_flags[i].name[0]) || \
                (GROUP##_flags[i].variant & TWO && argv[j][1] == '-' \
                 && !strcmp(GROUP##_flags[i].name, argv[j]+2))) \
            { \
                found = 1; \
                int retval = GROUP##_flags[i].func(0, NULL); \
                if (GROUP##_flags[i].returns) return retval; \
            } \
        } \
        \
        if (!found) \
        { \
            fprintf(stderr, NAME ": '%s' is not a flag\n", argv[j]); \
            return 0; \
        } \
    }

/*
 * the body is split up so we can construct our own group function for
 * e.g. ARGV0 parsing
 */
#define COMMAND_GROUP_BODY(GROUP, FIRST, SECOND) \
    if (argc > 1) \
    { \
        COMMAND_GROUP_BODY_##FIRST(GROUP) \
        COMMAND_GROUP_BODY_##SECOND(GROUP) \
        \
        fprintf(stderr, NAME ": '%s' is not a command\n", argv[1]); \
        return 0; \
    } \
    return GROUP##_help(argc-1, argv+1);

/*
 * the main command group function is only suppose to find given command
 * by the name and then invoke it.
 *
 * If the desired command is not found we should tell the user that.
 *
 * If no command is provided we should just print the list of commands by
 * calling the groups help command.
 */
#define COMMAND_GROUP_FUNC(GROUP) \
    COMMAND_GROUP(GROUP) \
    { \
        COMMAND_GROUP_BODY(GROUP, FLAGS, COMMANDS) \
    } \


#endif
