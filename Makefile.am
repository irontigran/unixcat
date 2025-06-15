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
		tests/pass-fds.sh \
		tests/udp-basic-send-recv.sh \
		tests/protocol-mutual-exclusion.sh \
		tests/udp-pass-fds.sh
dist_check_SCRIPTS = $(TESTS) tests/test-lib.sh
if HAVE_SEQPACKET
TESTS += tests/seq-basic-send-recv.sh \
		 tests/seq-pass-fds.sh
else !HAVE_SEQPACKET
endif !HAVE_SEQPACKET

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

sending_tests = tests/creds-send-always-recv-always.sh \
				tests/creds-send-once-recv-always.sh \
				tests/creds-send-once-recv-once.sh
nonsending_tests = tests/creds-send-none-recv-always.sh \
				   tests/creds-send-none-recv-once.sh \
				   tests/udp-creds-recv.sh
if HAVE_SEQPACKET
nonsending_tests += tests/seq-creds-recv.sh
else !HAVE_SEQPACKET
endif !HAVE_SEQPACKET

if SCM_CREDENTIALS
ucat_SOURCES += src/creds/scmcredentials.c
TESTS += $(sending_tests) $(nonsending_tests)
TESTS += tests/creds-send-always-recv-always-with-pid.sh \
		 tests/creds-send-once-recv-always-with-pid.sh \
		 tests/creds-send-always-recv-none-linux.sh \
		 tests/creds-send-always-recv-once-linux.sh \
		 tests/creds-send-once-recv-none-linux.sh
else !SCM_CREDENTIALS
if SCM_CREDS
TESTS += $(nonsending_tests)
if LOCAL_CREDS_PERSISTENT
ucat_SOURCES += src/creds/scmcreds_persistent.c
else !LOCAL_CREDS_PERSISTENT
ucat_SOURCES += src/creds/scmcreds_nonpersistent.c
endif !LOCAL_CREDS_PERSISTENT
if SEND_CREDS
TESTS += $(sending_tests)
TESTS += tests/creds-send-always-recv-none-bsd.sh \
		 tests/creds-send-always-recv-once-bsd.sh \
		 tests/creds-send-once-recv-none-bsd.sh
ucat_SOURCES += src/creds/scmcreds_send.c
else !SEND_CREDS
ucat_SOURCES += src/creds/scmcreds_nosend.c
endif !SEND_CREDS
else !SCM_CREDS
ucat_SOURCES += src/creds/creds_disabled.c
endif !SCM_CREDS
endif !SCM_CREDENTIALS

if HAVE_PASSSEC
ucat_SOURCES += src/security/enabled.c
TESTS += tests/pass-seccxt.sh
else
ucat_SOURCES += src/security/disabled.c
endif !HAVE_PASSSEC
