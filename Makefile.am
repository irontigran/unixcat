AM_CPPFLAGS = -I$(srcdir)/include

bin_PROGRAMS = ucat
ucat_SOURCES = src/cli.c src/help.c src/main.c src/serv.c \
			   man/long.h2m
noinst_HEADERS = include/cli.h include/help.h include/printfd.h include/main.h include/serv.h

if HAVE_FGETPATH
ucat_SOURCES += src/printfd_getpath.c
else
if HAVE_PROCFS
ucat_SOURCES += src/printfd_linux.c
else
if HAVE_KINFO
ucat_SOURCES += src/printfd_kinfo.c
else
ucat_SOURCES += src/printfd_default.c
endif
endif
endif

dist_man1_MANS = ucat.1
CLEANFILES = ucat.1
ucat.1: $(srcdir)/man/long.h2m $(srcdir)/src/help.c $(srcdir)/src/main.c
	-$(HELP2MAN) --no-discard-stderr --no-info --output=$@ --include=$< ./ucat

TESTS = tests/basic-send-recv.sh \
		tests/pass-fds.sh
dist_check_SCRIPTS = $(TESTS)

