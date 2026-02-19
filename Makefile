.PHONY : all

all:
	$(MAKE) -f Makefile.ac
	$(MAKE) -f Makefile.rf2
	$(MAKE) -f Makefile.pcars2
	$(MAKE) -f Makefile.r3e

debug:
	$(MAKE) debug -f Makefile.ac
	$(MAKE) debug -f Makefile.rf2
	$(MAKE) debug -f Makefile.pcars2
	$(MAKE) debug -f Makefile.r3e

clean:
	rm -f assets/*
