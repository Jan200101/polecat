# polecat

- *What is polecat?*
polecat is wine manager 

- *Why?*
I'm not satified with the quality of lutris and how its maintained.
It feels like almost every week there is a regression or other problem caused by not enough testing and sanity checking being done.
A week before I made polecat I fed up with Lutris constantly breaking on me and having to wait for a fix to land on the master branch
I looked up how lutris fetched wine and DXVK versions and implemented them in C with libcurl to download stuff, json-c to parse the API (and later a config file) and libarchive to unpack the downloaded archives for wine and DXVK.

- *Why should I use polecat instead of XYZ?*
You probably shouldn't.
Polecat is intended to be used by power-users that know how to deal with wine willing to troubleshoot when shit goes down.

- *What about installers/other lutris features*
lutris support is planned to be added but there is a lot to deal with that is easier to do in a higher level language like Python.

## Dependencies

polecat depends on:

- libcurl
- json-c
- libarchive
- cmake

### [License](LICENSE)

polecat is licensed under MIT so feel free to do what you want with it