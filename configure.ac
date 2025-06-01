#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([ucat], [0.1.0],,, [https://github.com/irontigran/unixcat])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_MISSING_PROG(HELP2MAN, help2man)

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h
                  stdint.h
                  sys/param.h
                  sys/socket.h
                  unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UINT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_PID_T
AC_TYPE_UID_T

# Checks for library functions.
AC_CHECK_FUNCS([memmove memset socket strchr])
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Custom checks

# Various ways to try and get a path from a file descriptor.
# Some/most BSDs have an F_GETPATH for fcntl.
AC_MSG_CHECKING([for F_GETPATH])
AC_COMPILE_IFELSE(
                  [AC_LANG_PROGRAM(
                   [#include <fcntl.h>],
                   [int cmd = F_GETPATH;])],
                  [AC_MSG_RESULT([yes])
                  have_fgetpath=yes],
                  [AC_MSG_RESULT([no])
                  have_fgetpath=no])
AM_CONDITIONAL([HAVE_FGETPATH], [test "x$have_fgetpath" = "xyes"])
# Linux has procfs.
AC_MSG_CHECKING([for procfs])
if test -d /proc && test -r /proc/meminfo; then
    have_procfs=yes
    AC_MSG_RESULT([yes])
else
    have_procfs=no
    AC_MSG_RESULT([no])
fi
AM_CONDITIONAL([HAVE_PROCFS], [test "x$have_procfs" = "xyes"])
# FreeBSD has KINFO for fcntl.
AC_MSG_CHECKING([for KINFO])
AC_COMPILE_IFELSE(
                  [AC_LANG_PROGRAM(
                   [#include <fcntl.h>],
                   [int cmd = F_KINFO;])],
                  [AC_MSG_RESULT([yes])
                  have_kinfo=yes],
                  [AC_MSG_RESULT([no])
                  have_kinfo=no])
AM_CONDITIONAL([HAVE_KINFO], [test "x$have_kinfo" = "xyes"])
# End file descriptor tests.

# Tests for what the system allows passing credentials.
AC_MSG_CHECKING([if passing credentials is allowed])
AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([
        #define _GNU_SOURCE
        #include <sys/socket.h>
    ], [
        int opt = SCM_CREDENTIALS;
    ])], [
    AC_MSG_RESULT([Linux-style allows all features])
    scm_credentials=yes], [
    AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([
            #include <sys/socket.h>
        ], [int opt = SCM_CREDS; struct sockcred cred;])
        ], [
        AC_MSG_RESULT([BSD-style credentials])
        scm_creds=yes
        AC_MSG_CHECKING([if sending credentials is allowed])
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM([
                #include <sys/socket.h>
            ], [struct cmsgcred creds;])
            ], [
            AC_MSG_RESULT([send and receive])
            send_creds=yes
            ], [
            AC_MSG_RESULT([receive only])
            send_creds=no
            ])
        AC_MSG_CHECKING([if option for receiving persistent credentials])
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM([
                #include <sys/un.h>
            ], [int opt = LOCAL_CREDS_PERSISTENT;])
            ], [
            AC_MSG_RESULT([yes])
            local_creds_persistent=yes
            ], [
            AC_MSG_RESULT([no])
            local_creds_persistent=no
            ])
        ], [
        AC_MSG_RESULT([disabled])
        creds_disabled=yes
        ])
    ]
)
        

AM_CONDITIONAL([SCM_CREDENTIALS], [test "x$scm_credentials" = "xyes"])
AM_CONDITIONAL([SCM_CREDS], [test "x$scm_creds" = "xyes"])
AM_CONDITIONAL([SEND_CREDS], [test "x$send_creds" = "xyes"])
AM_CONDITIONAL([LOCAL_CREDS_PERSISTENT], [test "x$local_creds_persistent" = "xyes"])
AM_CONDITIONAL([CREDS_DISABLED], [test "x$creds_disabled" = "xyes"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
