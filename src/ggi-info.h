
/* -*- Mode: C; c-base-offset: 4 -*-
 * vim: tabstop=4 shiftwidth=4 expandtab
 */

#ifndef __GUILE_GOBJECT_INTROSPECTION_INFO_H__
#define __GUILE_GOBJECT_INTROSPECTION_INFO_H__

#include <glib.h>
#include <libguile.h>
#include <girepository.h>

G_BEGIN_DECLS

void _ggi_finalize_info (SCM scm_info);

gsize _ggi_g_type_tag_size (GITypeTag type_tag);
gsize _ggi_g_type_info_size (GITypeInfo *type_info);

GIBaseInfo * ggi_object_get_gi_info (SCM scm_object);

gchar * _ggi_g_base_info_get_fullname (GIBaseInfo *info);

SCM ggi_make_info (GIBaseInfo *info);

void ggi_info_early_init (void);
void ggi_info_init (void);

G_END_DECLS

#endif /* __GGI_INFO_H__ */
