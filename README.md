# polecat

- *What is polecat?*    
polecat is wine manager that manages, installs and downloads wine for you.
It also works with Valves Proton and just requires you to drag drop the Proton Wine into data directory as output by `polecat env`

- *Why should I use polecat instead of Lutris/XYZ?*    
You probably shouldn't.
Polecat is intended to be used by power-users that know how to do things by hand and deal with them when they break.

- *What about installers*    
Installers are planned and on their way

## Installation

### Fedora
polecat is available as a [COPR repository](https://copr.fedorainfracloud.org/coprs/sentry/polecat/)    
`sudo dnf copr enable sentry/polecat`    
`sudo dnf install polecat`

### Arch
polecat is available on the AUR
use your favorite AUR manager to install it


## Dependencies

polecat depends on:

- libcurl
- json-c
- libarchive
- cmake

### [License](LICENSE)

polecat is licensed under MIT so feel free to do what you want with it