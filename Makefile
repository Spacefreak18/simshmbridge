all: assets/createsimshm assets/simshmbridge.exe assets/bridge #assets/shmtool.exe # assets/shmwrapper3.exe # assets/shmwrapperwintest.exe

CC=gcc
WINECC=i686-w64-mingw32-gcc
CFLAGS=-Wall -Os -g

assets/createsimshm: createsimshm.c
	$(CC) $< $(CFLAGS) -o $@

assets/simshmbridge.exe: simshmbridge.c
	$(WINECC) $< $(CFLAGS) -mconsole -o $@

assets/bridge: bridge.c
	$(CC) $< $(CFLAGS) -o $@

#assets/shmtool.exe: shmtool.c
#	$(WINECC) $< $(CFLAGS) -mconsole -o $@

clean:
	rm -f assets/createsimshm assets/simshmbridge.exe assets/bridge.c

# vim: syntax=make ts=4 sw=4 sts=4 sr noet
