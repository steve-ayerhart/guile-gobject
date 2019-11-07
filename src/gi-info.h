/* -*- Mode: C; c-base-offset: 4 -*-
 * vim: tabstop=4 shiftwidth=4 expandtab
 */

#ifndef __GUILE_GOBJECT_INTROSPECTION_INFO_H__
#define __GUILE_GOBJECT_INTROSPECTION_INFO_H__

#include <glib.h>
#include <libguile.h>
#include <girepository.h>

G_BEGIN_DECLS

void _scm_c_gi_finalize_info (SCM scm_info);

gsize _scmgi_c_g_type_tag_size (GITypeTag type_tag);
gsize _scmgi_c_g_type_info_size (GITypeInfo *type_info);

GIBaseInfo * scm_object_get_gi_info (SCM scm_object);

gchar * _scmgi_c_g_base_info_get_fullname (GIBaseInfo *info);

SCM scm_c_gi_make_info (GIBaseInfo *info);

void scm_c_gi_info_early_init (void);
void scm_c_gi_info_init (void);

G_END_DECLS

#endif /* __GUILE_GOBJECT_INTROSPECTION_INFO_H__ */
