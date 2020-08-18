#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "common.h"

static void getXDGDir(const char* envvar, const char* homeext, char* config, const size_t size)
{
    char* xdg_var = getenv(envvar);

    if (xdg_var)
    {
        strcpy(config, xdg_var);
    }
    else
    {
        char* home = getenv("HOME");
        strncpy(config, home, size);
        strncat(config, homeext, size - strlen(config));
    }
}

void getConfigDir(char* config, const size_t size)
{
    getXDGDir("XDG_CONFIG_HOME", "/.config/" NAME, config, size);
}

void getDataDir(char* config, const size_t size)
{
    getXDGDir("XDG_DATA_HOME", "/.local/share/" NAME, config, size);
}

void getCacheDir(char* config, const size_t size)
{
    getXDGDir("XDG_CACHE_HOME", "/.cache/" NAME, config, size);
}

void makeDir(const char* path)
{
    struct stat st = {0};

    if (stat(path, &st) == -1)
    {
        mkdir(path, 0755);
    }
}