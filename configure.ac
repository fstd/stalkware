#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([stalkware], [0.0.4], [van.fisted@googlemail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/stalkware.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([ext], [mandatory])
AC_PROG_CC
AC_PROG_MAKE_SET
#AC_PROG_INSTALL

#AC_HEADER_STDC

#AC_CHECK_HEADERS([stdbool.h stdlib.h string.h unistd.h])
AC_CHECK_HEADER_STDBOOL
AC_CHECK_FUNCS([strchr strdup strndup strtol memset])

#AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AC_FUNC_STRTOD
#AC_CHECK_FUNCS([getline])
AC_CHECK_LIB([srsbsns], [ringbuf_init], [], [AC_MSG_ERROR([libsrsbsns not installed?])])
AC_CHECK_LIB([srsirc], [irc_init], [], [AC_MSG_ERROR([libsrsirc not installed?])])
AC_CHECK_LIB([tw], [free], [], [AC_MSG_ERROR([libtw not installed?])])


AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo \
"-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'.
 C Compiler: '${CC} ${CFLAGS}'
 C++ Compiler: '${CXX} ${CXXFLAGS}'
 Libraries: '${LIBS}'

--------------------------------------------------"
