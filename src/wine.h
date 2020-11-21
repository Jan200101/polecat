#ifndef WINE_H
#define WINE_H

#define WINEBIN "/bin/wine"
#define PROTONBIN "/dist/bin/wine"

// enum type used to represent if the installed wine version is proton or normal wine
// this is mainly used to know wheither to call WINEBIN or PROTONBIN
enum wine_type_t {
	WINE_NONE,
    WINE_NORMAL,
    WINE_PROTON
};

int wine(int, char**);
int wine_download(int, char**);
int wine_list(int, char**);
int wine_run(int, char**);
int wine_installed(int, char**);
int wine_help(int, char**);

enum wine_type_t check_wine_ver(char*, size_t);

#endif