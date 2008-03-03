## Process this file with automake to produce Makefile.in

INCLUDES = \
	@GNOMEUI_CFLAGS@ \
	-DDATADIR=\""$(datadir)"\"

bin_PROGRAMS = gmanedit

gmanedit_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

gmanedit_LDADD = \
	@GNOMEUI_LIBS@

