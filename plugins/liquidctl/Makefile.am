plugindir = $(libdir)/mate-sensors-applet/plugins

# libsensors plugin
AM_CPPFLAGS = -DMATELOCALEDIR=\""$(datadir)/locale/"\" \
	-DG_LOG_DOMAIN=\""sensors-applet"\" \
	-DPIXMAPS_DIR=\""$(datadir)/pixmaps/$(PACKAGE)/"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DLIBDIR=\""$(libdir)"\" \
        -DSYSCONFDIR=\""$(sysconfdir)"\" \
        -DPREFIX=\""$(prefix)"\" \
	-I$(top_srcdir) \
	$(GLIB_CFLAGS) \
	$(WARN_CFLAGS)

plugin_LTLIBRARIES = libliquidctl.la

noinst_PROGRAMS = testlibliquidctl


libliquidctl_la_SOURCES =	\
	liquidctl-plugin.h	\
	liquidctl-plugin.c

libliquidctl_la_LDFLAGS =      \
    -l:libmate-sensors-applet-plugin.so \
    $(PLUGIN_LIBTOOL_FLAGS) \
    $(GLIB_LIBS) \
    -lc

# libliquidctl_la_LIBADD = /usr/lib/x86_64-linux-gnu/libmate-sensors-applet-plugin.so

testlibliquidctl_SOURCES =     \
	liquidctl-plugin.c     \
	testlibliquidctl.c

testlibliquidctl_CFLAGS = -DTEST_PROGRAMM
testlibliquidctl_LDFLAGS = $(GLIB_LIBS) -lc

-include $(top_srcdir)/git.mk
