#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>

#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define WINE_API "https://lutris.net/api/runners/wine"
#define DXVK_API "https://api.github.com/repos/lutris/dxvk/releases"
#define INSTALLER_API "https://lutris.net/api/installers/"

#ifndef NAME
#warning "no name specified, setting it to polecat"
#define NAME "polecat"
#endif

#ifndef VERSION
#warning "no version specified, setting it to 0.0.0"
#define VERSION "0.0.0"
#endif

#define USER_AGENT NAME "/" VERSION

#define USAGE_STR "Usage: " NAME

// inspired by Zigs unreachable
#include <assert.h>
#define unreachable assert(0 && "unreachable code reached")

struct MemoryStruct {
    uint8_t* memory;
    size_t size;
};

struct Command {
    char* name;
    int (*func)(int, char**);
    char* description;
};

void print_help(const struct Command*, size_t);

struct stat getStat(const char* path);
bool isFile(const char*);
bool isDir(const char*);

void makeDir(const char* path);

#endif
