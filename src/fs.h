#ifndef FS_H
#define FS_H

#ifdef __cplusplus
extern "C" {
#endif

#define OS_PATH_SEP '/'

int isFile(const char*);
int isDir(const char*);

int makeDir(const char*);
int removeDir(const char*);

#ifdef __cplusplus
}
#endif

#endif