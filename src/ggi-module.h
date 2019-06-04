/* -*- Mode: C; c-basic-offset: 4 -*-
 */
#ifndef __GGI_MODULE_H__
#define __GGI_MODULE_H__


#include <girepository.h>
#include <glib-object.h>
#include <libguile.h>

G_BEGIN_DECLS

SCM
ggi_import_by_gi_info (SCM scm_info);

SCM
ggi_c_import_by_gi_info (GIBaseInfo *info);

SCM
ggi_import_by_name (const char *namespace_,
                    const char *name);

SCM
ggi_import_by_g_type (GType gtype);

void
ggi_module_init (void);

G_END_DECLS


#endif
