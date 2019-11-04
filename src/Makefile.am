gobjectlib_LTLIBRARIES = gobject.la

AM_CFLAGS = -I. $(WARN_CFLAGS) $(DEBUG_FLAGS) -fPIC

C_FILES = \
	utils.c \
	gtype.c \
	gvalue.c \
	gclosure.c \
	gsignal.c \
	gparameter.c \
	gobject.c \
	ggi-argument.c \
	ggi-array.c \
	ggi-basic-types.c \
	ggi-cache.c \
	ggi-enum-marshal.c \
	ggi-invoke.c \
	ggi-closure.c \
	ggi-list.c \
	ggi-error.c \
	ggi-hashtable.c \
	ggi-object.c \
	ggi-value.c \
	introspection.c \
	gi-info.c \
	gi-repository.c

H_FILES = $(C_FILES:.c=.h)
X_FILES = $(C_FILES:.c=.x)

gobject_la_SOURCES = $(C_FILES) $(H_FILES) private.h ggi-invoke-state-struct.h

gobject_la_CFLAGS = $(GUILE_CFLAGS) $(GIR_CFLAGS) $(GLIB_CFLAGS) $(AM_CFLAGS)
gobject_la_LIBADD = $(GUILE_LIBS) $(GIR_LIBS) $(GLIB_LIBS)
gobject_la_LDFLAGS = -module -avoid-version

MAINTERCLEANFILES = Makefile.in

SUFFIXES = .x .c .scm

.c.x:
	guile-snarf $(gobject_la_CFLAGS) $< > $@ || { rm $@; false; }

BUILT_SOURCES = $(X_FILES) g-object/config.scm

gobjectlibdir = $(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/extensions

GOBJECTS = $(SOURCES:%.scm=%.go) gobject.la

g-object/config.scm: Makefile g-object/config.scm.in
	mkdir -p g-object
	sed -e "s|@gobjectlibdir\@|$(gobjectlibdir)|" \
		$(srcdir)/g-object/config.scm.in > g-object/config.scm

EXTRA_DIST = $(SOURCES)
GUILE_WARNINGS = -Wunbound-variable -Warity-mismatch -Wformat

moddir=$(prefix)/share/guile/site/$(GUILE_EFFECTIVE_VERSION)
godir=$(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/ccache

SOURCES = \
	g-object/introspection.scm \
	g-object/introspection/info.scm \
	g-object/introspection/repository.scm \
	g-object.scm

CLEANFILES = $(GOBJECTS) $(X_FILES) g-object/config.scm
