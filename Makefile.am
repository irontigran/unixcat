AM_CPPFLAGS = -I$(srcdir)/include

bin_PROGRAMS = ucat
ucat_SOURCES = src/fdstate.c src/help.c src/main.c src/net.c src/options.c \
			   man/long.h2m
noinst_HEADERS = include/creds.h \
				 include/fdstate.h \
				 include/help.h \
				 include/printfd.h \
				 include/main.h \
				 include/net.h \
				 include/options.h

dist_man1_MANS = ucat.1
CLEANFILES = ucat.1
ucat.1: $(srcdir)/man/long.h2m $(srcdir)/src/help.c $(srcdir)/src/main.c
	-$(HELP2MAN) --no-discard-stderr --no-info --output=$@ --include=$< ./ucat

TESTS = tests/basic-send-recv.sh \
		tests/pass-fds.sh
dist_check_SCRIPTS = $(TESTS) tests/test-lib.sh

if HAVE_FGETPATH
ucat_SOURCES += src/printfd/printfd_getpath.c
else
if HAVE_PROCFS
ucat_SOURCES += src/printfd/printfd_procfs.c
else
if HAVE_KINFO
ucat_SOURCES += src/printfd/printfd_kinfo.c
else
ucat_SOURCES += src/printfd/printfd_default.c
endif
endif
endif

if SCM_CREDENTIALS
ucat_SOURCES += src/creds/scmcredentials.c
TESTS += tests/pass-scmcredentials.sh \
		tests/creds-send-none-recv-once.sh \
		tests/creds-send-none-recv-always.sh \
		tests/creds-linux-send-once-recv-none.sh \
		tests/creds-send-once-recv-once.sh \
		tests/creds-send-always-recv-always.sh
else
if SCM_CREDS
if LOCAL_CREDS_PERSISTENT
ucat_SOURCES += src/creds/scmcreds_persistent.c
TESTS += tests/pass-scmcreds-persistent.sh
else
ucat_SOURCES += src/creds/scmcreds_nonpersistent.c
TESTS += tests/pass-scmcreds-nonpersistent.sh
endif
if SEND_CREDS
ucat_SOURCES += src/creds/scmcreds_send.c
TESTS += tests/creds-send-none-recv-once.sh \
		tests/creds-send-none-recv-always.sh \
		tests/creds-bsd-send-once-recv-none.sh \
		tests/creds-send-once-recv-once.sh \
		tests/creds-send-always-recv-always.sh
else
ucat_SOURCES += src/creds/scmcreds_nosend.c
TESTS += tests/creds-send-none-recv-once.sh \
		tests/creds-send-none-recv-always.sh
endif
else
ucat_SOURCES += src/creds/creds_disabled.c
endif
endif


