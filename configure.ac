#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libcfgparse], [0.10])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT([disable-shared])

# Checks for programs.
AC_PROG_CC([gcc cc])
AC_PROG_CC_C99
if test -z "$CC" || test "X$CC" = "Xno"; then
  AC_MSG_ERROR([No suitable C99 compiler found])
fi
AM_PROG_CC_C_O

# add -Wall if possible
AX_CHECK_COMPILER_FLAGS([-Wall], [CFLAGS="$CFLAGS -Wall"])

# Check for debugging 
AC_ARG_ENABLE(debug, 
  AS_HELP_STRING([--disable-debug], [Turn off debugging]), 
  [case "${enableval}" in 
     yes) debug=true ;; 
     no)  debug=false ;; 
     *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;; 
   esac], 
  [debug=true]) 
 
AS_IF([test "$debug" = "false"], [CPPFLAGS="${CPPFLAGS} -DNDEBUG"]) 

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo ""
echo "                       Configure Summary"
echo "                       ================="
echo ""
echo "Compiling Options:"
echo "------------------"
echo "                     C Compiler: $CC"
echo "                       CPPFLAGS: $CPPFLAGS"
echo "                         CFLAGS: $CFLAGS"
echo "                        LDFLAGS: $LDFLAGS"
echo "                           LIBS: $LIBS"
