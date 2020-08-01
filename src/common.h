#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

#define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])

#define WINE_API "https://lutris.net/api/runners/wine"
#define DXVK_API "https://api.github.com/repos/lutris/dxvk/releases"

#define USER_AGENT NAME "/" VERSION

#define USAGE_STR "Usage: " NAME

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

#endif