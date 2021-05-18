#ifndef MAIN_H
#define MAIN_H

#include "command.h"

#define WINE_PREFIX "wine-"

COMMAND(main, env);
COMMAND(main, version);
COMMAND(main, help);

#endif
