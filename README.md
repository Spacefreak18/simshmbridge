# Sim Shared Memory Bridge Utility for Linux

Wrapper programs to map shared memory from Linux for access by Wine and Proton.

## FAQ

### Why is this necessary?

Many games write out data (telemetry data, etc) for use by third party applications. Memory mapped files are used as a means of inter process
communication in this situation due to it's superior performance. Wine and Proton games are running inside their compatibility layer and using
Windows APIs to create memory mapped files inside of that compatibility layer. This set of utilities will help bridge these memory mapped files
into unix space for the games it supports.

### Why doesn't wine just forward the windows mapping calls into Unix Space itself?

Good question. I suppose it could, and I think it should, but it doesn't. I may resurrect this discussion below about this exact matter.
[Accessing shared memory from Linux](https://bugs.winehq.org/show_bug.cgi?id=54015)

### Why not udp?

udp will trash your frame rate. Some games only support udp, and support is being added for them as well, they do not need these tools.

### Alright, then how does this tool work?

There are two methods of acheiving this bridging.

Method 1: Run a helper process inside the target wine/proton prefix that creates the memory mapped file before the game does, and we give it the
unix space location. [poljar](https://github.com/poljar/shm-bridge) has a great explanation with a graphic of what exactly is happening.

Method 2: If method 1 does not work, method 2 should always work, though it is slower and more cumbersome. It relies on the fact that child processes can [inherit handles](https://learn.microsoft.com/en-us/windows/win32/procthread/inheritance) including memory mapped files
from their parent process.

In both situations ( although not strictly necessary in method 1 especially ), I start a process on the unix side (*shm for example acshm) which
creates the shared memory file first so it is controlled natively on the unix side first, and can be managed appropriately. So it is necessary
to start *shm before anything else.

### Cool, how do I use this?

Compile, follow the example, and add the utilities to your launch command or start them manually at the appropriate time.

### How do I close these cleanly?

Ctrl-C will close these cleanly. If you don't have the terminal available, send ```kill -2 $(pid)``` to the parent process. The one caveat is that
it's tough to close the child processes in method 2 cleanly. They will close, just I don't have the ability to catch a signal and release things. Which
doesn't really matter in this situation, all it does have file descriptors open which will be cleaned up when the process anyway.

### Sounds a little cumbersome, do you have plans to make it easier?

I do, while embedding workarounds into tools like monocoque or simmonitor isn't ideal, I have another project called [simd](https://github.com/spacefreak18/simapi/tree/master/simd). simd creates zeroed stubs
of all supported memory mapped files so there is no need for (*shm). Furthermore simd is aware when a supported sim starts, so it could detect when
the sim starts and run a user configured command when appropriate. monocoque and simmonitor are simd aware, but it do not require simd so that
other techniques of bridging the memory can be susbstituted without breaking the application.

### Are there any alternatives?

Yes, now there are atleast some alternatives. Check out [DataLink](https://github.com/LukasLichten/Datalink) and [shm-bridge](https://github.com/poljar/shm-bridge) for rust-based solutions.
The goal is the tools that use the memory such as [monocoque](https://github.com/Spacefreak18/monocoque), [simmonitor](https://github.com/Spacefreak18/simmonitor), or [DataRace](https://github.com/LukasLichten/Datalink) remain agnostic to how that data was mapped over.

### So if I use simd I don't need to run the *shm executable first?

Correct, although you should still start simd before anything else. It is possible to have simd start on boot, I haven't tested this as I spend
a lot of time debugging and making all this better.

### One last overview?

+ *shm - creates memory mapped files in unixspace
+ *bridge.exe - method 1, opens memory mapped file in windows space ahead of game, backed by unix, so we can have access.
+ *handle.exe - method 2, opens memory mapped file in windows space after the game, forwards the handle to the memory mapped file(s) to helper process 2
+ *handle - method 2, process 2, writes from the received file descriptor from windows space, into the corresponding memory mapped file in unix space.

(the asterisk here is a place holder for the corresponding supported sim, one of "ac", "pcars2", "rf2")

### I thought RFactor2 related sims had a native plugin?

They do, but rf2bridge.exe could still be useful if one wanted to map the shared memory from unix space back into a wine space that had a program such as
CrewChief running.

## Compilation

### Prerequisits

This project is used by my Monocoque project which is a device manager for Simulators. It depends on [simapi](https://github.com/spacefreak18/simapi) for the headers for the various simulators that are supported. When pulling lastest if the submodule does not download run:
```
git submodule sync --recursive
git submodule update --init --recursive
```
### Compile All
```
make clean; make
```
### Compile for Assetto Corsa and Assetto Corsa Competizione
```
make -f Makefile.ac
```
### Compile Project Cars 2 and Automobilista 2
```
make -f Makefile.pcars2
```
This will compile 3 separate files:
createsimshm - creates linux shared memory files that can be mapped into wine
simshmbridge.exe - bridges shared memory between wine and Linux ( always needed )
bridge - manually moves memory into Linux shared memory for Simulators that do no like it if simshmbridge.exe creates and owns the shared memory file

## Basic Mapping Examples

### Assetto Corsa (with Method 1 in Launch Command)
First in a separate terminal or tab start ./acshm (or simd).

Start the game with a modified launch command like the following
```
%command% & sleep 5 && ~/.steam/steam/steamapps/common/Proton\ 8.0/proton run ~/shmwrap/ac/acbridge.exe
```

you can exit acshm by pressing "q" and acbridge.exe by pressing ctrl-c

### Assetto Corsa (with Method 2)
(look in the examples folder for a self contained script)
First in a separate terminal or tab start ./acshm (or simd).

```
protontricks --no-runtime --background-wineserver -c "wine ./simshmbridge/assets/achandle.exe ./simshmbridge/assets/achandle" 244210
```

you can exit acshm by pressing "q" and achandle.exe by pressing ctrl-c

### Automobilista (with Method 2)
(look in the examples folder for a self contained shell script)
First in a separate terminal or tab start ./pcars2shm (or simd).

and in another terminal or tab run
```
protontricks --no-runtime --background-wineserver -c "wine ./simshmbridge/assets/pcars2handle.exe ./simshmbridge/assets/pcars2handle" 1066890
```
(as you can see, it's possible for simd to take care of all of this in the future)
again, you can exit pcars2shm with "q" and pcars2handle.exe with ctrl-c

## Mapping from Linux Shared Memory back to Wine

This is useful if there is another app such as Crewchief that uses Shared Memory, and you'd like to have a separate dedicated wine prefix just for Crewchief.

For example, for Assetto Corsa, rename acbridge.exe to acs.exe, then
```
CREWCHIEF_WINEPREFIX="${HOME}/.wine"

export WINEPREFIX="${CREWCHIEF_WINEPREFIX}"
export WINEARCH="win64"

wine ~/shmwrap/ac/acs.exe
```
make sure createsimshm is running and the shared memory files are in /dev/shm.
then you can start crewchief however you are starting crewchief. One could even add this to a launcher, but you'd need separate launchers for each sim. Crewchief
looks for certain executable names to know if the sim is running. To determine the name of the executable, I read the code [here](https://gitlab.com/mr_belowski/CrewChiefV4/-/blob/master/CrewChiefV4/GameDefinition.cs)

## ToDo
 - Support more sims/programs
