.PHONY : all

all:
	$(MAKE) -f Makefile.ac
	$(MAKE) -f Makefile.pcars2

clean:
	rm -f assets/*
