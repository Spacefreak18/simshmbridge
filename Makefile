.PHONY : all

all:
	$(MAKE) -f Makefile.ac
	$(MAKE) -f Makefile.rf2
	$(MAKE) -f Makefile.pcars2
	$(MAKE) -f Makefile.r3e
	$(MAKE) -f Makefile.lmu

debug:
	$(MAKE) debug -f Makefile.ac
	$(MAKE) debug -f Makefile.rf2
	$(MAKE) debug -f Makefile.pcars2
	$(MAKE) debug -f Makefile.lmu

clean:
	rm -f assets/*
