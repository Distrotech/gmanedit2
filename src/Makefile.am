## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GNOMEUI_CFLAGS@ \
	-DDATADIR=\""$(datadir)"\"

bin_PROGRAMS = gmanedit

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

gmanedit_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

gmanedit_LDADD = \
	@GNOMEUI_LIBS@

