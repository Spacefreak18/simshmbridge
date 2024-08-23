all: assets/createsimshm assets/simshmbridge.exe assets/bridge

CC=gcc
WINECC=i686-w64-mingw32-gcc
CFLAGS=-Wall -Os -g

assets/createsimshm: createsimshm.c
	$(CC) $< $(CFLAGS) -o $@

assets/simshmbridge.exe: simshmbridge.c
	$(WINECC) $< $(CFLAGS) -mconsole -o $@

assets/bridge: bridge.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f assets/createsimshm assets/simshmbridge.exe assets/bridge.c

# vim: syntax=make ts=4 sw=4 sts=4 sr noet
