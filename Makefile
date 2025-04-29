.PHONY : all

all:
	$(MAKE) -f Makefile.ac
	$(MAKE) -f Makefile.rf2
	$(MAKE) -f Makefile.pcars2

debug:
	$(MAKE) debug -f Makefile.ac
	$(MAKE) debug -f Makefile.rf2
	$(MAKE) debug -f Makefile.pcars2

clean:
	rm -f assets/*
