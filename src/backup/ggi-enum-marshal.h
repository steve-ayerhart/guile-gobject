/* -*- Mode: C; c-basic-offset: 4 -*-
 * vim: tabstop=4 shiftwidth=4 expandtab
 */

#ifndef __GUILE_GOBJECT_INTROSPECTION_ENUM_MARHSAL_H__
#define __GUILE_GOBJECT_INTROSPECTION_ENUM_MARSHAL_H__

#include "ggi-cache.h"

#include <girepository.h>

GGIArgCache *
ggi_arg_enum_new_from_info (GITypeInfo      *type_info,
                            GIArgInfo       *arg_info,
                            GITransfer       transfer,
                            GGIDirection     direction,
                            GIInterfaceInfo *iface_info);

GGIArgCache *
ggi_arg_flags_new_from_info (GITypeInfo      *type_info,
                             GIArgInfo       *arg_info,
                             GITransfer       transfer,
                             GGIDirection     direction,
                             GIInterfaceInfo *iface_info);

G_BEGIN_DECLS


#endif
