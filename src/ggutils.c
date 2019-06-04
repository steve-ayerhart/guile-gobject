#include "ggutils.h"
/*
SCM
ggerror_to_scm (GError *gerror)
{
  return scm_list_3 (scm_from_ulong (gerror->domain),
                     scm_from_ulong (gerror->code),
                     scm_from_locale_string (gerror->message));
}

void
ggthrow_gerror (GError *gerror)
{
  SCM scm_gerror = ggerror_to_scm (gerror);

  g_error_free (gerror);

  scm_throw (scm_from_locale_symbol ("g-error"), scm_gerror);
}
*/

void
ggutils_init (void)
{
  #ifndef SCM_MAGIC_SNARFER
  #include "ggutils.x"
  #endif
}
