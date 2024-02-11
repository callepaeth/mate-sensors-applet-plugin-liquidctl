# ================= initialization =================== #

AC_INIT([mate-sensors-applet-plugin-liquidctl], [0.86.0], [https://github.com/callepaeth/mate-sensors-applet-plugin-liquidctl/issues],
        [mate-sensors-applet-plugin-liquidctl], [https://mate-desktop.org])

AC_CONFIG_SRCDIR([plugins/liquidctl/liquidctl-plugin.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-xz no-dist-gzip check-news])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE
MATE_COMPILE_WARNINGS

# ============== basic compiler settings ============= #
AC_PROG_CC
AC_HEADER_STDC

# =================== for plugins ==================== #
AC_PROG_LIBTOOL

# ========== export compiler / linker options ======== #
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBS)

# ============== look for dependencies =============== #
# check for headers needed for standard interfaces
AC_CHECK_HEADERS(
	stdlib.h \
	string.h \
	stdio.h \
	sys/types.h \
	ctype.h
)

GLIB_REQUIRED=2.50.0
LIBMATE_SENSORS_APPLET_PLUGIN_REQUIRED=1.26

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gio-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)


# AC_CHECK_HEADER([mate-sensors-applet/sensors-applet-plugin.h], [], [
#   AC_MSG_ERROR([required header mate-sensors-applet/sensors-applet-plugin.h not found])
# ])

# AC_SEARCH_LIBS([sensors_applet_plugin_add_sensor], [libmate-sensors-applet-plugin], [], [
#   AC_MSG_ERROR([required library libmate-sensors-applet-plugin not found])
# ])

# ================ yelp-tools stuff ============= #
YELP_HELP_INIT

# ================ GSettings stuff ============= #
GLIB_GSETTINGS

# ==================== plugin stuff ================== #
PLUGIN_LIBTOOL_FLAGS="-module -avoid-version"
AC_SUBST(PLUGIN_LIBTOOL_FLAGS)

# ================= generate files =================== #
AC_CONFIG_FILES([
	Makefile
	plugins/Makefile
	plugins/liquidctl/Makefile
])
AC_OUTPUT

dnl ---------------------------------------------------------------------------
dnl - Show summary
dnl ---------------------------------------------------------------------------

echo "
       mate-sensors-applet-plugin-liquidctl $VERSION
       `echo mate-sensors-applet-plugin-liquidctl $VERSION | sed "s/./=/g"`

       prefix:               ${prefix}
       compiler:             ${CC}
       cflags:               ${CFLAGS}
       warning flags:        ${WARN_CFLAGS}
"
