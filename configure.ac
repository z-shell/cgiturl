dnl -*- Mode: Autoconf; sh-indentation: 2; indent-tabs-mode: nil; sh-basic-offset: 2; -*-
dnl vim:ft=config:sw=2:sts=2:et
dnl
dnl License -> cgiturl.git/LICENSE
dnl

AC_INIT(VATS)
AC_PREREQ(2.59c)
AC_CONFIG_HEADER(config.h)

dnl Version
. ${srcdir}/Config/version.mk
echo "configure runs for VATS $VERSION"
echo "THIS CONFIGURE SCRIPT IS FOR TESTING (VATS) FRAMEWORK ONLY"

dnl Which machine
AC_CANONICAL_HOST

dnl C compiler
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CPP
AC_C_CONST

test -z "${CFLAGS+set}"  && CFLAGS=  self_cflags=1
test -z "${LDFLAGS+set}" && LDFLAGS= self_ldflags=1

dnl if user hasn't specified CFLAGS, then for
dnl gcc use: -O2 -g and warning flags, and for
dnl other compiler use: -O

if test -n "$self_cflags"; then
  if test -n "$GCC"; then
    CFLAGS="$CFLAGS -Wall -g -Wmissing-prototypes -O2"
  else
    CFLAGS="$CFLAGS -O"
  fi
fi

if test -n "$self_ldflags"; then
  case "$host_os" in
    aix*|hpux*|netbsd*|openbsd*) ;;  # "ld -g" is not valid on these systems
    darwin*) LDFLAGS=-Wl,-x ;;
    *)       LDFLAGS=-g ;;
  esac
fi

AC_SUBST(CFLAGS)dnl
AC_SUBST(LDFLAGS)dnl

dnl
dnl Programs
dnl

AC_PROG_MAKE_SET            dnl Make defines $MAKE?
AC_PROG_INSTALL             dnl BSD compatible `install'
AC_PROG_AWK                 dnl mawk, gawk, nawk, then awk
AC_PROG_LN                  dnl Working ln
AC_PROG_LN_S                dnl ln -s/ln/cp
AC_PROG_EGREP               dnl Sets $EGREP

AC_CHECK_PROGS([TEXI2DVI], [texi2dvi], [: texi2dvi])
AC_CHECK_PROGS([TEXI2PDF], [texi2pdf], [: texi2pdf])
AC_CHECK_PROGS([TEXI2HTML], [texi2any texi2html], [: texi2html])

if test x"$TEXI2PDF" != xtexi2pdf && test x"$TEXI2DVI" = xtexi2dvi; then
  TEXI2PDF='texi2dvi --pdf'
fi

if test x"$TEXI2HTML" = xtexi2any; then
  TEXI2HTML='texi2any -c TEXI2HTML=1'
fi

case "$LC_PAPER" in
  ??_US*) PAPERSIZE=us ;;
  *)      PAPERSIZE=a4 ;;
esac
AC_SUBST(PAPERSIZE)

dnl
dnl mk files
dnl

CONFIG_MK="${srcdir}/Config/config.mk"
CONFIG_VATS_MK="${srcdir}/Config/config_vats.mk"
CLEAN_MK="${srcdir}/Config/clean.mk"
DEFINITIONS_MK="Config/definitions.mk"
VERSION_MK="${srcdir}/Config/version.mk"

AC_SUBST_FILE(CONFIG_MK)dnl
AC_SUBST_FILE(CONFIG_VATS_MK)dnl
AC_SUBST_FILE(CLEAN_MK)dnl
AC_SUBST_FILE(DEFINITIONS_MK)dnl
AC_SUBST_FILE(VERSION_MK)dnl

dnl
dnl Final
dnl

AC_CONFIG_FILES(Config/definitions.mk VATSMakefile VATS/Makefile)
AC_CONFIG_COMMANDS([stamp-h], [echo >stamp-h])

AC_OUTPUT

echo "
VATS configuration
------------------
VATS version              : ${VERSION}
host operating system     : ${host_cpu}-${host_vendor}-${host_os}
source code location      : ${srcdir}
preprocessor flags        : ${CPPFLAGS}
executable compiler flags : ${CFLAGS}
executable linker flags   : ${LDFLAGS}
library flags             : ${LIBS}
"

exit 0
