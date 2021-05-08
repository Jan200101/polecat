
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "common.h"

#ifdef _WIN32
#define HOMEVAR "USERPROFILE"
#else
#define HOMEVAR "HOME"
#endif

static void getXDGDir(const char* envvar, const char* homeext, char* config, const size_t size)
{
    char* xdg_var = getenv(envvar);

    if (xdg_var)
    {
        strncpy(config, xdg_var, size);
    }
    else
    {
        char* home = getenv(HOMEVAR);
        if (!home) home = "";
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

void getWineDir(char* config, const size_t size)
{
    getDataDir(config, size);
    strncat(config, "/wine", size - strlen(config) - 1);
}

void getDXVKDir(char* config, const size_t size)
{
    getDataDir(config, size);
    strncat(config, "/dxvk", size - strlen(config) - 1);
}
