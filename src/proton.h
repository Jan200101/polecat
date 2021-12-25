#ifndef PROTON_H
#define PROTON_H

#include <stddef.h>

#include "command.h"

extern char* repos[];

COMMAND_GROUP(proton);
COMMAND(proton, download);
COMMAND(proton, list);
COMMAND(proton, installed);
COMMAND(proton, remove);
COMMAND(proton, help);

#endif