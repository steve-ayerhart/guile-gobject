#ifndef __GGUTILS_H__
#define __GGUTILS_H__

#include <glib.h>
#include <libguile.h>
#include <girepository.h>

G_BEGIN_DECLS

/*
#define SCM_TO_GPOINTER(scm) ((gpointer) SCM_UNPACK (scm))
#define GPOINTER_TO_SCM(gptr) (SCM_PACK ((scm_t_bits) gptr))

SCM
ggerror_to_scm (GError *gerror);

void
ggthrow_gerror (GError *gerror);
*/
void
ggutils_init (void);

G_END_DECLS

#endif
