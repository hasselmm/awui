AM_CFLAGS  = -Wall -Werror -Wmissing-prototypes
AM_LDFLAGS =

noinst_LTLIBRARIES = libawui-model.la \
		     libawui-web.la \
		     libawui-hildon.la
noinst_PROGRAMS = $(TESTS) src/awui-hildon-showcase src/captcha-decode
bin_PROGRAMS = src/awui-hildon

if GDLIB
noinst_PROGRAMS += src/captcha-font
endif

###################
# --- cleanup --- #
###################

BUILT_SOURCES =
CLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES) Makefile.in
DISTCLEANFILES = $(BUILT_SOURCES)

###########################
# --- additional data --- #
###########################

EXTRA_DISTFILES = bin/mkenum.in bin/mkheader

############################
# --- libawui-model.la --- #
############################

libawui_model_la_CFLAGS = $(AM_CFLAGS) $(LIBAWUI_MODEL_CFLAGS)
libawui_model_la_LIBADD = $(AM_LDFLAGS) $(LIBAWUI_MODEL_LIBS)

libawui_model_la_headers = $(libawui_model_la_static_headers) \
			   libawui/model/enums.h \
			   libawui/model.h
libawui_model_la_SOURCES = $(libawui_model_la_static_sources) \
			   $(libaw_model_la_headers) \
			   libawui/model/enums.c

libawui_model_la_static_headers = libawui/model/fleet.h \
				  libawui/model/item.h \
				  libawui/model/news.h \
				  libawui/model/planet.h \
				  libawui/model/settings.h \
				  libawui/model/science.h \
				  libawui/model/star.h
libawui_model_la_static_sources = libawui/model/fleet.c \
				  libawui/model/item.c \
				  libawui/model/news.c \
				  libawui/model/planet.c \
				  libawui/model/settings.c \
				  libawui/model/science.c \
				  libawui/model/star.c

libawui/model.h: libawui/model.h-stamp
libawui/model.h-stamp: libawui/model.h.in libawui/model/enums.h $(libawui_model_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkheader libawui/model.h $^

libawui/model/enums.h: libawui/model/enums.h-stamp
libawui/model/enums.h-stamp: libawui/model/enums.h.in $(libawui_model_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkenum libawui/model/enums.h $^

libawui/model/enums.c: libawui/model/enums.c-stamp
libawui/model/enums.c-stamp: libawui/model/enums.c.in $(libawui_model_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkenum libawui/model/enums.c $^

BUILT_SOURCES += libawui/model/enums.c \
	         libawui/model/enums.h \
	         libawui/model.h
CLEANFILES += libawui/model/enums.c-stamp \
	      libawui/model/enums.h-stamp \
	      libawui/model.h-stamp

##########################
# --- libawui-web.la --- #
##########################

libawui_web_la_CFLAGS = $(AM_CFLAGS) $(LIBAWUI_WEB_CFLAGS)
libawui_web_la_LIBADD = $(AM_LDFLAGS) $(LIBAWUI_WEB_LIBS) libawui-model.la

libawui_web_la_headers = $(libawui_web_la_static_headers) libawui/web.h
libawui_web_la_SOURCES = $(libawui_web_la_static_sources) $(libawui_web_la_headers) 

libawui_web_la_static_headers = libawui/web/captcha.h \
				libawui/web/parser.h \
				libawui/web/session.h
libawui_web_la_static_sources = libawui/web/captcha.c \
				libawui/web/parser.c \
				libawui/web/session.c

libawui/web.h: libawui/web.h-stamp
libawui/web.h-stamp: libawui/web.h.in $(libawui_web_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkheader libawui/web.h $^

libawui/web/captcha-font.h: data/captcha-font.png
	$(AM_V_GEN) gdk-pixbuf-csource --name=aw_captcha_font $< > $@

BUILT_SOURCES += libawui/web.h \
		 libawui/web/captcha-font.h

CLEANFILES += libawui/web.h-stamp

#############################
# --- libawui-hildon.la --- #
#############################

libawui_hildon_la_CFLAGS = $(AM_CFLAGS) $(LIBAWUI_HILDON_CFLAGS)
libawui_hildon_la_LIBADD = $(AM_LDFLAGS) $(LIBAWUI_HILDON_LIBS) libawui-model.la

libawui_hildon_la_headers = $(libawui_hildon_la_static_headers) \
			    libawui/hildon/enums.h \
			    libawui/hildon.h
libawui_hildon_la_SOURCES = $(libawui_hildon_la_static_sources) \
			    $(libawui_hildon_la_headers) \
			    libawui/hildon/enums.c

libawui_hildon_la_static_headers = libawui/hildon/captcha-dialog.h \
				   libawui/hildon/fleet-view.h \
				   libawui/hildon/login-dialog.h \
				   libawui/hildon/main-view.h \
				   libawui/hildon/map-view.h \
				   libawui/hildon/marshal.h \
				   libawui/hildon/news-view.h \
				   libawui/hildon/planet-view.h \
				   libawui/hildon/planet-detail-view.h \
				   libawui/hildon/popup-dialog.h \
				   libawui/hildon/science-view.h \
				   libawui/hildon/system-view.h \
				   libawui/hildon/tree-view.h \
				   libawui/hildon/window.h
libawui_hildon_la_static_sources = libawui/hildon/captcha-dialog.c \
				   libawui/hildon/fleet-view.c \
				   libawui/hildon/login-dialog.c \
				   libawui/hildon/main-view.c \
				   libawui/hildon/map-view.c \
				   libawui/hildon/marshal.c \
				   libawui/hildon/news-view.c \
				   libawui/hildon/planet-view.c \
				   libawui/hildon/planet-detail-view.c \
				   libawui/hildon/popup-dialog.c \
				   libawui/hildon/science-view.c \
				   libawui/hildon/system-view.c \
				   libawui/hildon/tree-view.c \
				   libawui/hildon/window.c

libawui/hildon.h: libawui/hildon.h-stamp
libawui/hildon.h-stamp: libawui/hildon.h.in libawui/hildon/enums.h $(libawui_hildon_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkheader libawui/hildon.h $^

libawui/hildon/enums.h: libawui/hildon/enums.h-stamp
libawui/hildon/enums.h-stamp: libawui/hildon/enums.h.in $(libawui_hildon_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkenum libawui/hildon/enums.h $^

libawui/hildon/enums.c: libawui/hildon/enums.c-stamp
libawui/hildon/enums.c-stamp: libawui/hildon/enums.c.in $(libawui_hildon_la_static_headers)
	$(AM_V_GEN) $(srcdir)/bin/mkenum libawui/hildon/enums.c $^

libawui/hildon/marshal.h: libawui/hildon/marshal.list
	$(AM_V_GEN) @GLIB_GENMARSHAL@ --prefix=aw_cclosure_marshal --header < $< > $@

libawui/hildon/marshal.c: libawui/hildon/marshal.list
	$(AM_V_GEN) ( echo '#include "marshal.h"'; \
	              @GLIB_GENMARSHAL@ --prefix=aw_cclosure_marshal --body < $< ) > $@

BUILT_SOURCES += libawui/hildon/enums.c \
	         libawui/hildon/enums.h \
	         libawui/hildon.h
CLEANFILES += libawui/hildon/enums.c-stamp \
	      libawui/hildon/enums.h-stamp \
	      libawui/hildon.h-stamp

EXTRA_DISTFILES += libawui/hildon/marshal.list

####################
# --- programs --- #
####################

src_awui_hildon_CFLAGS = $(AM_CFLAGS) $(LIBAWUI_HILDON_CFLAGS) $(LIBAWUI_WEB_CFLAGS)
src_awui_hildon_LDADD = $(AM_LDFLAGS) $(LIBAWUI_HILDON_LIBS) $(LIBAWUI_WEB_LIBS) libawui-hildon.la libawui-web.la libawui-model.la
src_awui_hildon_SOURCES = src/captcha.c \
			  src/login.c \
			  src/main.c \
			  src/main.h

src_awui_hildon_showcase_CFLAGS  = $(AM_CFLAGS) $(LIBAWUI_HILDON_CFLAGS)
src_awui_hildon_showcase_LDADD   = $(AM_LDFLAGS) $(LIBAWUI_HILDON_LIBS) libawui-web.la libawui-hildon.la
src_awui_hildon_showcase_SOURCES = src/showcase.c

if GDLIB
src_captcha_font_CFLAGS  = $(AM_CFLAGS) $(GDLIB_CFLAGS)
src_captcha_font_LDADD   = $(AM_LDFLAGS) $(GDLIB_LDFLAGS) $(GDLIB_LIBS)
src_captcha_font_SOURCES = src/captcha-font.c
endif

src_captcha_decode_CFLAGS  = $(AM_CFLAGS) $(LIBAWUI_WEB_CFLAGS)
src_captcha_decode_LDADD   = $(AM_LDFLAGS) $(LIBAWUI_WEB_LIBS) libawui-web.la
src_captcha_decode_SOURCES = src/captcha-decode.c

############################
# --- regression tests --- #
############################

TESTS = tests/test-archive \
	tests/test-parser
TESTS_PATH = /
TESTS_ENVIRONMENT = gtester -p=$(TESTS_PATH)

tests_test_archive_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS)
tests_test_archive_LDADD = $(AM_LDFLAGS) $(GLIB_LIBS) -lbz2

tests_test_parser_CFLAGS = $(AM_CFLAGS) $(LIBAWUI_WEB_CFLAGS)
tests_test_parser_LDADD = $(AM_LDFLAGS) $(LIBAWUI_WEB_LIBS) libawui-web.la libawui-model.la

##################################
# --- maemo launcher support --- #
##################################

if MAEMO_LAUNCHER
libawui_model_la_CFLAGS += $(MAEMO_LAUNCHER_CFLAGS)
libawui_model_la_LIBADD += $(MAEMO_LAUNCHER_LIBS)

libawui_web_la_CFLAGS += $(MAEMO_LAUNCHER_CFLAGS)
libawui_web_la_LIBADD += $(MAEMO_LAUNCHER_LIBS)

libawui_hildon_la_CFLAGS += $(MAEMO_LAUNCHER_CFLAGS)
libawui_hildon_la_LIBADD += $(MAEMO_LAUNCHER_LIBS)

src_awui_hildon_CFLAGS += $(MAEMO_LAUNCHER_CFLAGS)
src_awui_hildon_LDADD  += $(MAEMO_LAUNCHER_LIBS)
endif

desktopdir=$(datadir)/applications/hildon
desktop_DATA=data/awui-hildon.desktop

