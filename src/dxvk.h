#ifndef DXVK_H
#define DXVK_H

#define DXVKSETUP "/setup_dxvk.sh"

#include "command.h"

COMMAND_GROUP(dxvk);
COMMAND(dxvk, download);
COMMAND(dxvk, remove);
COMMAND(dxvk, list);
COMMAND(dxvk, install);
COMMAND(dxvk, installed);
COMMAND(dxvk, help);

#endif