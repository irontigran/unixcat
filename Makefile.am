AM_CPPFLAGS = -I$(srcdir)/include

bin_PROGRAMS = ucat
ucat_SOURCES = src/cli.c src/help.c src/main.c src/serv.c
noinst_HEADERS = include/cli.h include/help.h include/serv.h

dist_man1_MANS = ucat.1
CLEANFILES = ucat.1
ucat.1: man/long.h2m src/help.c src/main.c
	-$(HELP2MAN) --no-discard-stderr --no-info --output=$@ --include=$< ./ucat
