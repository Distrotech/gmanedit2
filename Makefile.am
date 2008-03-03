## Process this file with automake to produce Makefile.in

SUBDIRS = po src

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d ../pixmaps; then \
	  $(mkinstalldirs) $(datadir)/pixmaps; \
	  for pixmap in ../pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(datadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

man_MANS = gmanedit.1

DESKTOP_FILES = gmanedit.desktop

desktopdir = $(datadir)/applications
desktop_DATA = $(DESKTOP_FILES)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath $(DESKTOP_FILES) gmanedit.1
