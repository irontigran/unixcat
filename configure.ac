#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([ucat], [0.1.0], [https://github.com/irontigran/unixcat])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_MISSING_PROG(HELP2MAN, help2man)

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h
                  stdint.h
                  sys/socket.h
                  unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset socket])
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC

# Custom tests
AC_MSG_CHECKING([for procfs])
if test -d /proc && test -r /proc/meminfo; then
    have_procfs=yes
    AC_MSG_RESULT([yes])
else
    have_procfs=no
    AC_MSG_RESULT([no])
fi
AM_CONDITIONAL([HAVE_PROCFS], [test "x$have_procfs" = "xyes"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
