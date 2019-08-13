# the library's version
VERSION:=0.8.6
PACKAGE_TARNAME:=tinydtls

# tools

SHELL = /bin/sh
MKDIR = mkdir
DOXYGEN= /usr/bin/doxygen

top_builddir = ..
prefix = /usr/local
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
htmldir = ${docdir}

DISTDIR?=$(top_builddir)/tinydtls-0.8.6
FILES:=Makefile.in Doxyfile.in html

doc:	Doxyfile
	$(DOXYGEN) $< >./doxygen.out 2>&1 

clean:
	@rm -rf html

distclean:	clean
	@rm -rf $(DISTDIR)
	@rm -f *~ 

dist:	doc
	test -d $(DISTDIR)/doc || mkdir $(DISTDIR)/doc
	cp -r $(FILES) $(DISTDIR)/doc

install:	$(doc) html
	test -d $(htmldir) || mkdir -p $(htmldir)
	cp -r html/* $(htmldir)
