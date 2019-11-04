/* -*- Mode: C; c-basic-offset: 4 -*-
 * vim: tabstop=4 shiftwidth=4 expandtab
 */

#ifndef __GUILE_GOBJECT_INTROSPECTION_OBJECT_H__
#define __GUILE_GOBJECT_INTROSPECTION_OBJECT_H__

#include "ggi-cache.h"

#include <girepository.h>

G_BEGIN_DECLS

gboolean
ggi_arg_gobject_out_from_scm (SCM         scm_arg,
                              GIArgument *arg,
                              GITransfer  transfer);

SCM
ggi_arg_gobject_to_scm (GIArgument *arg,
                        GITransfer  transfer);

SCM
ggi_arg_gobject_to_scm_called_from_c (GIArgument *arg,
                                      GITransfer  transfer);

GGIArgCache *
ggi_arg_gobject_new_from_info (GITypeInfo       *type_info,
                               GIArgInfo        *arg_info,
                               GITransfer        transfer,
                               GGIDirection      direction,
                               GIInterfaceInfo  *iface_info,
                               GGICallableCache *callable_cache);

G_END_DECLS

#endif
