# the library's version
VERSION:=0.8.6

# tools

SHELL = /bin/sh
MKDIR = mkdir

top_builddir = ..

THIS=platform-specific
DISTDIR?=$(top_builddir)/tinydtls-0.8.6
FILES:=Makefile.in $(wildcard *.h)

clean:

distclean:	clean
	@rm -rf $(DISTDIR)
	@rm -f *~ 

dist:	
	test -d $(DISTDIR)/$(THIS) || mkdir $(DISTDIR)/$(THIS)
	cp -r $(FILES) $(DISTDIR)/$(THIS)

# this directory contains no installation candidates
install:
	:
