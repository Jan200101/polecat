# polecat

- *What is polecat?*
polecat is wine manager 

- *why?*
I used to use Lutris to manage installed Wine games but after
the lutris team had released a stable version with a program
breaking regression and the master branch was working as well
as a alpha branch I decided enough is enough and started working
on my own.

- *Why should I use polecat instead of X*
maybe you are tired of lutris breaking like me or maybe you
just don't want to use lutris because its a bloated python
application running GTK with [CSD](https://en.wikipedia.org/wiki/Client-side_decoration)

- *Can I help?*
If you use polecat you are already more than I could ask for
but if you want to go the extra step you can go ahead and
look at the [roadmap](https://github.com/Jan200101/polecat/issues/1) and suggest or implement things

## Dependencies

polecat depends on:

- pkg-config
- libcurl
- curl-config (should be included with the libcurl development package)
- json-c
- libarchive

## Build instructions

- ensure you have all [dependencies](#Dependencies) installed 
- run `make` for a debug build
- run `make TARGET=release` for a release build


### [License](LICENSE)

polecat is licensed under MIT so feel free to do what you want with it