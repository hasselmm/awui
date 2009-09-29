AC_INIT([awui],[0.0],[http://github.com/hasselmm/awui/])
AM_INIT_AUTOMAKE([check-news dist-bzip2 subdir-objects -Wall -Werror])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AM_PATH_GLIB_2_0([2.20.3])

PKG_CHECK_MODULES([LIBAWUI_MODEL],  [gobject-2.0        >= 2.20.3])
PKG_CHECK_MODULES([LIBAWUI_WEB],    [gdk-pixbuf-2.0     >= 2.14.7
                                     libsoup-2.4        >= 2.24.3])
PKG_CHECK_MODULES([LIBAWUI_HILDON], [hildon-1           >= 1.1.96])
PKG_CHECK_MODULES([MAEMO_LAUNCHER], [maemo-launcher-app >= 0.35])

dnl
dnl check for maemo launcher
dnl

AC_ARG_ENABLE([maemo-launcher],
              [AS_HELP_STRING([--enable-maemo-launcher],
                              [build with Maemo launcher acceleration])])
AM_CONDITIONAL([MAEMO_LAUNCHER], [test "$enable_maemo_launcher" == "yes"])

dnl
dnl check for gdlib
dnl

AC_ARG_VAR([GDLIB_CONFIG])
AC_PATH_PROG([GDLIB_CONFIG], [gdlib-config])
AM_CONDITIONAL([GDLIB], [test -n "$GDLIB_CONFIG"])

if test -n "$GDLIB_CONFIG"; then
  AC_SUBST([GDLIB_CFLAGS],   [`$GDLIB_CONFIG --cflags`])
  AC_SUBST([GDLIB_LDFLAGS],  [`$GDLIB_CONFIG --ldflags`])
  AC_SUBST([GDLIB_LIBS],     [`$GDLIB_CONFIG --libs`])
fi

dnl
dnl check for libbz2
dnl

AC_CHECK_HEADERS([bzlib.h])
AC_CHECK_LIB([bz2],[BZ2_bzDecompressInit])

dnl
dnl write configuration status
dnl

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile data/awui-hildon.desktop])
AC_CONFIG_FILES([bin/mkenum],[chmod +x bin/mkenum])

AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$PACKAGE"],["Message domain"])

AC_OUTPUT
