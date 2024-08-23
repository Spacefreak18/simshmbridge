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
### Assetto Corsa and Assetto Corsa Competizione
```
make CFLAGS=-DASSETTOCORSA
```
### Project Cars 2 and Automobilista 2
```
make CFLAGS=-DPROJECTCARS2
```
This will compile 3 separate files:
createsimshm - creates linux shared memory files that can be mapped into wine
simshmbridge.exe - bridges shared memory between wine and Linux ( always needed )
bridge - manually moves memory into Linux shared memory for Simulators that do no like it if simshmbridge.exe creates and owns the shared memory file

## Basic Mapping Examples

The best way is to run the wrapper from the Proton Launch Command.
### Assetto Corsa and Assetto Corsa Competizione
```
%command% & ~/shmwrap/ac/createsimshm & sleep 5 && ~/.steam/steam/steamapps/common/Proton\ 8.0/proton run ~/shmwrap/ac/simshmbridge.exe
```
### Project Cars 2 and Automobilista 2
```
%command% & sleep 30 && ~/.steam/steam/steamapps/common/Proton\ 8.0/proton run ~/shmwrap/pcars2/simshmbridge.exe ~/shmwrap/pcars2/bridge
```
### Manually
```
./createsimshm
```
in a separate terminal
```
protontricks --no-runtime --background-wineserver -c "wine /path/to/simshmbridge.exe" 244210
```
you can exit createsimshm by pressing "q" and simshmbridge.exe by pressing ctrl-c

## Mapping from Linux Shared Memory back to Wine

This is useful if there is another app such as Crewchief that uses Shared Memory, and you'd like to have a separate dedicated wine prefix just for Crewchief.

For example, for Assetto Corsa, rename simshmbridge.exe to acs.exe, then
```
CREWCHIEF_WINEPREFIX="${HOME}/.wine"

export WINEPREFIX="${CREWCHIEF_WINEPREFIX}"
export WINEARCH="win64"

wine /home/racerx/shmwrap/ac/acs.exe
```
make sure createsimshm is running and the shared memory files are in /dev/shm.
then you can start crewchief however you are starting crewchief. One could even add this to a launcher, but you'd need separate launchers for each sim. Crewchief
looks for certain executable names to know if the sim is running. To determine the name of the executable, I read the code [here](https://gitlab.com/mr_belowski/CrewChiefV4/-/blob/master/CrewChiefV4/GameDefinition.cs)

## ToDo
 - Support more sims/programs

