all: assets/createsimshm assets/simshmbridge.exe # assets/shmwrapper3.exe # assets/shmwrapperwintest.exe

CC=gcc
WINECC=i686-w64-mingw32-gcc
CFLAGS=-Wall -Os -g

assets/createsimshm: createsimshm.c
	$(CC) $< $(CFLAGS) -o $@

assets/simshmbridge.exe: simshmbridge.c
	$(WINECC) $< $(CFLAGS) -mconsole -o $@

clean:
	rm -f assets/createsimshm assets/simshmbridge.exe

# vim: syntax=make ts=4 sw=4 sts=4 sr noet
