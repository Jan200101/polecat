#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

void getConfigDir(char* config);
void getDataDir(char* config);

void makeDir(const char* path);

#endif