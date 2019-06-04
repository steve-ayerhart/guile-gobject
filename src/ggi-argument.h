#ifndef __GGI_ARGUMENT_H
#define __GGI_ARGUMENT _H

#include <girffi.h>
#include <glib.h>
#include <libguile.h>
#include <girepository.h>

G_BEGIN_DECLS

G_GNUC_INTERNAL SCM
ggi_arg_to_scm (GITypeInfo *arg_type,
                GITransfer transfer_type,
                GIArgument arg);

G_END_DECLS

#endif
