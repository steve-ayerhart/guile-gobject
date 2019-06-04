/* -*- Mode: C; c-basic-offset: 4 -*-
 */
#ifndef __GGOBJECT_GTYPE_H__
#define __GGOBJECT_GTYPE_H__

#include <glib-object.h>
#include <libguile.h>

G_BEGIN_DECLS

#define SET_GTYPE_SYMBOL(sym, val) scm_symbol_pset_x (sym, scm_from_ulong (val))

static SCM scmg_type_metaclass;

static SCM scm_make_class;

SCM
scmg_type_wrapper_new (GType gtype);

void
scmg_type_init (void);

G_END_DECLS


#endif
