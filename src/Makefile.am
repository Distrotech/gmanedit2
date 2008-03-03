## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GTK_CFLAGS@ \
	-DDATADIR=\""$(datadir)"\" \
	-DLOCALEDIR=\""$(datadir)/locale"\"

bin_PROGRAMS = gmanedit

gmanedit_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

gmanedit_LDADD = \
	@GTK_LIBS@

