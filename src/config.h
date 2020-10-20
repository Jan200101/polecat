#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

void getConfigDir(char*, const size_t);
void getDataDir(char*, const size_t);
void getCacheDir(char*, const size_t);

void getWineDir(char*, const size_t);
void getDXVKDir(char*, const size_t);

#endif
