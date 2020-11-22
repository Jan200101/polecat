#ifndef DXVK_H
#define DXVK_H

#define DXVKSETUP "/setup_dxvk.sh"

int dxvk(int, char**);
int dxvk_download(int, char**);
int dxvk_install(int, char**);
int dxvk_installed(int, char**);
int dxvk_list(int, char**);
int dxvk_help(int, char**);

#endif