# Sim Shared Memory Bridge Utility for Linux

Wrapper programs to map shared memory from Linux for access by Wine and Proton.

## Compilation

### Prerequisits

This project is used by my Monocoque project which is a device manager for Simulators. It depends on [simapi](https://github.com/spacefreak18/simapi) for the headers for the various simulators that are supported. When pulling lastest if the submodule does not download run:
```
git submodule sync --recursive
git submodule update --init --recursive
```
Then to compile wrapper for various shared memory files...
### Assetto Corsa
```
make CFLAGS=-DASSETTOCORSA
```
## Usage Example

```
./createsimshm
```
in a separate terminal
```
protontricks --no-runtime --background-wineserver -c "wine /path/to/simshmbridge.exe" 244210
```
you can exit createsimshm by pressing "q" and simshmbridge.exe by pressing ctrl-c

## ToDo
 - Support more sims/programs

