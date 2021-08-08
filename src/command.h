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
        return EXIT_SUCCESS; \
    }

/*
 * This is the same as the COMMAND macro except dedicated to the actual
 * command group name, which is the thing called from the actual main
 * function. 
 */
#define COMMAND_GROUP(GROUP) \
    int GROUP(int argc, char** argv)

/*
 * the body is split up so we can construct our own group function for
 * e.g. ARGV0 parsing
 */
#define COMMAND_GROUP_BODY(GROUP) \
    if (argc > 1) \
    { \
        for (int i = 1; i < argc; ++i) \
        { \
            for (unsigned long j = 0; j < ARRAY_LEN(GROUP##_commands); ++j) \
            { \
                if (!strcmp(GROUP##_commands[j].name, argv[i])) \
                { \
                    return GROUP##_commands[j].func(argc-i, argv+i); \
                } \
            } \
            \
            if (argv[i][0] != '-') continue; \
            \
            for (unsigned long j = 0; j < ARRAY_LEN(GROUP##_flags); ++j) \
            { \
                if ((GROUP##_flags[j].variant & ONE && argv[i][1] == GROUP##_flags[j].name[0] \
                                                    && argv[i][2] == '\0') || \
                    (GROUP##_flags[j].variant & TWO && argv[i][1] == '-' \
                     && !strcmp(GROUP##_flags[j].name, argv[i]+2))) \
                { \
                    int retval = GROUP##_flags[j].func(0, NULL); \
                    if (GROUP##_flags[j].returns) return retval; \
                } \
            } \
        } \
    } \
    return GROUP##_help(argc-1, argv+1); \


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
        COMMAND_GROUP_BODY(GROUP) \
    } \


#endif
