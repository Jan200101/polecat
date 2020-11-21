# polecat

- *What is polecat?*    
polecat is wine manager that manages, installs and downloads wine for you.
It also works with Valves Proton and just requires you to drag drop the Proton Wine into data directory as output by `polecat env`

- *Why?*    
I'm not satified with the quality of lutris and how its maintained.
It feels like almost every week there is a regression or other problem caused by not enough testing and sanity checking being done.
A week before I made polecat I fed up with Lutris constantly breaking on me and having to wait for a fix to land on the master branch
I looked up how lutris fetched wine and DXVK versions and implemented them in C with libcurl to download stuff, json-c to parse the API (and config files in the future), and libarchive to unpack the downloaded archives for wine and DXVK.

- *Why should I use polecat instead of Lutris/XYZ?*    
You probably shouldn't.
Polecat is intended to be used by power-users that know how to do things by hand and deal with them when they break.

- *What about installers/other lutris features*    
lutris support is planned and parsing of installers into an internal struct has already been added (as can be seen by invoking `polecat lutris info <installer-name>`) but it won't be here for a long time or until a large crowd of people convince me to do it faster.

## Dependencies

polecat depends on:

- libcurl
- json-c
- libarchive
- cmake

### [License](LICENSE)

polecat is licensed under MIT so feel free to do what you want with it