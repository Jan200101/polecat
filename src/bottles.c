
#include "bottles.h"

static const struct Command bottles_commands[] = {
    { .name = "info",    .func = bottles_info,    .description = "show information about a bottles script" },
};

static const struct Flag bottles_flags[] = {
    { .name = "help",       .variant = TWO, .returns = 1, .func = bottles_help,  .description = "show this message"},
    { .name = "no-net",     .variant = TWO, .returns = 0, .func = set_no_net,   .description = "run commands without commitment"}
};


COMMAND_GROUP_FUNC(bottles)

COMMAND(bottles, info)
{
    return 0;
}


COMMAND_HELP(bottles, " bottles")
