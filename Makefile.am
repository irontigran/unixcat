AM_CPPFLAGS = -I$(srcdir)/include

bin_PROGRAMS = ucat
ucat_SOURCES = src/help.c src/main.c src/sock.c
noinst_HEADERS = include/help.h include/sock.h

dist_man1_MANS = ucat.1
CLEANFILES = ucat.1
ucat.1: src/help.c src/main.c
	-$(HELP2MAN) --no-discard-stderr --no-info --output=$@ ./ucat
