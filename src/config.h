#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

void getConfigDir(char*, const size_t);
void getDataDir(char*, const size_t);
void getCacheDir(char*, const size_t);

void makeDir(const char* path);

#endif