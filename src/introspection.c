#include "introspection.h"

#include "gi-info.h"

SCM_DEFINE (scm_gobject_constant_from_info, "%constant_from_info", 1, 0, 0,
            (SCM scm_info),
            "")
{
  
}

void
scm_gobject_introspection_init (void)
{
  g_debug ("scm_gobject_introspection_init");

#ifndef SCM_MAGIC_SNARFER
#include "introspection.x"
#endif
}
