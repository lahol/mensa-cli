#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl configure.in
AC_PREREQ([2.63])
AC_INIT([Mensa CLI], [0.2], [holger.langenau@s2004.tu-chemnitz.de])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# no need to reconfigure if not explicitly ./configure --enable-maintainer-mode
# is run
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lc':
AC_CHECK_LIB([c], [memcpy])
AC_CHECK_LIB([termcap], [tgetent])

PKG_CHECK_MODULES(XML2, [libxml-2.0 >= 2.0.0])
PKG_CHECK_MODULES(CURL, [libcurl])

LIBS="$LIBS $XML2_LIBS"
CFLAGS="$CFLAGS $XML2_CFLAGS"

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h memory.h stdlib.h string.h fcntl.h])
AC_CHECK_HEADERS([curses.h term.h], [], [],
[[#ifdef HAVE_CURSES_H
#include <curses.h>
#endif
]])

AC_ARG_ENABLE([bashcompletion],
  [AS_HELP_STRING([--enable-bashcompletion],
    [Install bash completion script @<:@default=no@:>@])],
  [enable_bashcompletion=${enableval}],
  [enable_bashcompletion=no])

AM_CONDITIONAL([ENABLE_BASHCOMPLETION], [test x$enable_bashcompletion = xyes])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset setlocale strcasecmp strdup])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libmensa/Makefile
                 data/Makefile
                 data/bash_completion.d/Makefile
                 doc/manual/Makefile])
# create script
AC_CONFIG_FILES([scripts/mensa-init.sh], [chmod +x scripts/mensa-init.sh])

AC_OUTPUT

echo "-----------------------------------";

if test x$enable_bashcompletion = xyes;
then
  echo "Bash completion enabled";
else
  echo "Bash completion disabled";
fi

echo "Now type \`make' and then \`make install'";
