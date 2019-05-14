ggobjectlib_LTLIBRARIES = ggobject.la

AM_CFLAGS = -I. $(WARN_CFLAGS) $(DEBUG_FLAGS) -fPIC

C_FILES = \
	gtype.c \
	ggi-repository.c \
	ggi-module.c

H_FILES = $(C_FILES:.c=.h)
X_FILES = $(C_FILES:.c=.x)

ggobject_la_SOURCES = $(C_FILES) $(H_FILES)

ggobject_la_CFLAGS = $(GUILE_CFLAGS) $(GIR_CFLAGS) $(GLIB_CFLAGS) $(AM_CFLAGS)
ggobject_la_LIBADD = $(GUILE_LIBS) $(GIR_LIBS) $(GLIB_LIBS)
ggobject_la_LDFLAGS = -module -avoid-version

MAINTERCLEANFILES = Makefile.in

SUFFIXES = .x .c .scm

.c.x:
	guile-snarf $(ggobject_la_CFLAGS) $< > $@ || { rm $@; false; }

BUILT_SOURCES = $(X_FILES) g-object/config.scm

ggobjectlibdir = $(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/extensions

GOBJECTS = $(SOURCES:%.scm=%.go) ggobject.la

g-object/config.scm: Makefile g-object/config.scm.in
	mkdir -p g-object
	sed -e "s|@ggobjectlibdir\@|$(ggobjectlibdir)|" \
		$(srcdir)/g-object/config.scm.in > g-object/config.scm

EXTRA_DIST = $(SOURCES)
GUILE_WARNINGS = -Wunbound-variable -Warity-mismatch -Wformat

moddir=$(prefix)/share/guile/site/$(GUILE_EFFECTIVE_VERSION)
godir=$(libdir)/guile/$(GUILE_EFFECTIVE_VERSION)/ccache

SOURCES = \
	g-object/g-type.scm \
	g-object/introspection/repository.scm

CLEANFILES = $(GOBJECTS) $(X_FILES) g-object/config.scm
