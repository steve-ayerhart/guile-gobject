/* -*- Mode: C; c-basic-offset: 4 -*-
 */
#ifndef __GGOBJECT_H__
#define __GGOBJECT_H__

#include <glib.h>
#include <glib-object.h>
#include <libguile.h>

G_BEGIN_DECLS

static SCM
ggobject_get_direct_supers (GType gtype);

//static GType
//ggtype_from_object (SCM scm_object);

static SCM
ggtype_to_class (GType gtype);

static SCM
ggobject_lookup_class (GType gtype);

void
ggobject_init (void);

G_END_DECLS

#endif
