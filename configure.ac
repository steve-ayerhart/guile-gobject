AC_INIT([Guile Gobject], [0.1], [steve@ayerh.art],
[guile-gobject],
[https://bonerbonerboner.com])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

AC_PROG_CC

if test "x$GCC" = "xyes"; then
# Use compiler warnings.
WARN_CFLAGS="-Wall"
else
WARN_CFLAGS=""
fi
AC_SUBST([WARN_CFLAGS])

gl_EARLY
gl_INIT

dnl libtool
AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl guile
GUILE_PKG([2.2])
GUILE_PROGS
GUILE_FLAGS
GUILE_SITE_DIR
AC_SUBST(GUILE_LIBS)
AC_SUBST(GUILE_CFLAGS)

LT_INIT()

dnl introspection
PKG_CHECK_MODULES(GIR, gobject-introspection-1.0 >= 0.6.5)
AC_LIB_LINKFLAGS([girepository-1.0])
AC_SUBST(GIR_LIBS)
AC_SUBST(GIR_CFLAGS)

dnl glib
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.24.1])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)


AC_CONFIG_FILES([Makefile src/Makefile lib/Makefile])

AC_OUTPUT
