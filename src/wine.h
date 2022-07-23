#ifndef WINE_H
#define WINE_H

#define WINEBIN "/bin/wine"
#define PROTONBIN "/dist/bin/wine"

#include "command.h"

// enum type used to represent if the installed wine version is proton or normal wine
// this is mainly used to know wheither to call WINEBIN or PROTONBIN
enum wine_type_t {
	WINE_NONE,
    WINE_NORMAL,
    WINE_PROTON
};

COMMAND_GROUP(winecmd);
COMMAND(winecmd, download);
COMMAND(winecmd, remove);
COMMAND(winecmd, list);
COMMAND(winecmd, run);
COMMAND(winecmd, installed);
COMMAND(winecmd, env);
COMMAND(winecmd, shim);
COMMAND(winecmd, help);

enum wine_type_t check_wine_ver(char*, size_t);

#endif
