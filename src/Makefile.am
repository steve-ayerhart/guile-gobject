ggobjectlib_LTLIBRARIES = ggobject.la

AM_CFLAGS = -I. $(WARN_CFLAGS) $(DEBUG_FLAGS) -fPIC

C_FILES = \
	ggutils.c \
	gc.c \
	ggtype.c \
	ggvalue.c \
	ggsignal.c \
	ggclosure.c \
	ggparameter.c \
	ggobject.c \
	ggi-cache.c \
	ggi-argument.c \
	ggi-basic-types.c \
	ggi-value.c \
	ggi-object.c \
	ggi-enum-marshal.c \
	ggi-array.c \
	ggi-info.c \
	ggi-repository.c

H_FILES = $(C_FILES:.c=.h)
X_FILES = $(C_FILES:.c=.x)

ggobject_la_SOURCES = $(C_FILES) $(H_FILES) private.h ggi-invoke-state-struct.h

ggobject_la_CFLAGS = $(GUILE_CFLAGS) $(GIR_CFLAGS) $(GLIB_CFLAGS) $(AM_CFLAGS)
ggobject_la_LIBADD = $(GUILE_LIBS) $(GIR_LIBS) $(GLIB_LIBS)
ggobject_la_LDFLAGS = -module -avoid-version

MAINTERCLEANFILES = Makefile.in

SUFFIXES = .x .c .scm

.c.x:
	guile-snarf $(ggobject_la_CFLAGS) $< > $@ || { rm $@; false; }

BUILT_SOURCES = $(X_FILES) gobject/config.scm

ggobjectlibdir = $(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/extensions

GOBJECTS = $(SOURCES:%.scm=%.go) ggobject.la

gobject/config.scm: Makefile gobject/config.scm.in
	mkdir -p gobject
	sed -e "s|@ggobjectlibdir\@|$(ggobjectlibdir)|" \
		$(srcdir)/gobject/config.scm.in > gobject/config.scm

EXTRA_DIST = $(SOURCES)
GUILE_WARNINGS = -Wunbound-variable -Warity-mismatch -Wformat

moddir=$(prefix)/share/guile/site/$(GUILE_EFFECTIVE_VERSION)
godir=$(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/ccache

SOURCES = \
	gobject/gtype.scm \
	gobject/gvalue.scm \
	gobject/gsignal.scm \
	gobject/gparameter.scm \
	gobject/gclosure.scm \
	gobject/introspection.scm \
	gobject/introspection/info.scm \
	gobject/introspection/repository.scm \
	gobject.scm

CLEANFILES = $(GOBJECTS) $(X_FILES) gobject/config.scm
