#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>

#include "defines.h"

#include <json.h>

extern uint8_t no_net;

struct MemoryStruct {
    uint8_t* memory;
    size_t size;
};

struct Command {
    char* name;
    int (*func)(int, char**);
    char* description;
};

enum flag_variants {
	ONE = 1 << 0,
	TWO = 1 << 1,
	BOTH   = ONE + TWO,
};

struct Flag {
    char* name;
    enum flag_variants variant;
    uint8_t returns;
    int (*func)(int, char**);
    char* description;
};

int set_no_net(UNUSED int argc, UNUSED char** argv);

void print_help(const struct Command*, size_t, const struct Flag*, size_t);

struct stat getStat(const char* path);
int isFile(const char*);
int isDir(const char*);

int makeDir(const char* path);
int removeDir(const char *path);

#endif
