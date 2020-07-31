#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "common.h"

void getXDGDir(const char* envvar, const char* homeext, char* config)
{
    char* xdg_var = getenv(envvar);

    if (xdg_var)
    {
        strcpy(config, xdg_var);
    }
    else
    {
        char* home = getenv("HOME");
        strcpy(config, home);
        strcat(config, homeext);
    }
}

void getConfigDir(char* config)
{
    getXDGDir("XDG_CONFIG_HOME", "/.config/" NAME, config);
}

void getDataDir(char* config)
{
    getXDGDir("XDG_DATA_HOME", "/.local/share/" NAME, config);
}

void makeDir(const char* path)
{
    struct stat st = {0};

    if (stat(path, &st) == -1)
    {
        mkdir(path, 0755);
    }
}