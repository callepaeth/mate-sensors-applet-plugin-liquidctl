SUBDIRS = plugins

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

DISTCHECK_CONFIGURE_FLAGS = \
	--enable-compile-warnings=no \
	CFLAGS='-Wno-deprecated-declarations'

EXTRA_DIST = autogen.sh

DISTCLEANFILES	=

MSACLEANFILES = aclocal.m4 \
			compile \
			config.* \
			configure \
			depcomp \
			install-sh \
			ltmain.sh \
			missing \
			omf.make \
			xmldocs.make \
			INSTALL \
			ABOUT-NLS

msa-clean: maintainer-clean
	-test -z "$(MSACLEANFILES)" || rm -f $(MSACLEANFILES)
	-rm -rf m4;
	find . -name "Makefile.in" -exec rm \{\} \;

.PHONY: msa-clean

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

-include $(top_srcdir)/git.mk
