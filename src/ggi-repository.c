/* -*- Mode: C; c-basic-offset: 4 -*-
 */

#include "ggi-repository.h"
#include "ggi-info.h"

SCM_DEFINE (ggi_repository_require, "repository-require", 1, 2, 0,
            (SCM scm_namespace, SCM scm_version, SCM scm_lazy),
            ""
            )
{
  GIRepositoryLoadFlags flags = 0;
  GError *error;
  char *version;
  char *namespace;

  if (!SCM_UNBNDP (scm_lazy) && scm_is_true (scm_lazy)) {
    flags |= G_IREPOSITORY_LOAD_FLAG_LAZY;
  }

  scm_dynwind_begin (0);

  if (SCM_UNBNDP (scm_version))
    version = NULL;
  else
    version = scm_to_locale_string (scm_version);

  scm_dynwind_free (version);

  error = NULL;

  scm_namespace = scm_symbol_to_string (scm_namespace);
  namespace = scm_to_locale_string (scm_namespace);
  scm_dynwind_free (namespace);

  g_irepository_require (NULL, namespace, version, flags, &error);

  scm_dynwind_end ();

  if (error != NULL)
    {
      g_critical ("Failed to load typelib: %s", error->message);
      g_error_free (error);
    }

  return SCM_UNSPECIFIED;
}

SCM_DEFINE (ggi_repository_get_infos, "repository-get-infos", 1, 0, 0,
            (SCM scm_namespace),
            ""
            )
{
  GIBaseInfo *info;
  char *namespace;
  gssize n_infos;
  SCM scm_infos;
  gint i;

  scm_dynwind_begin (0);

  scm_namespace = scm_symbol_to_string (scm_namespace);
  namespace = scm_to_locale_string (scm_namespace);

  scm_dynwind_free (namespace);

  n_infos = g_irepository_get_n_infos (NULL, namespace);

  if (n_infos <0)
    {
      g_critical ("Namespace '%s' not loaded", namespace);
      return SCM_UNSPECIFIED;
    }

  scm_infos = SCM_EOL;

  for(i = 0; i < n_infos; i++) {
    SCM scm_info;

    info = g_irepository_get_info (NULL, namespace, i);
    g_assert (info != NULL);

    scm_info = ggi_make_info (info);

    scm_infos = scm_append (scm_list_2 (scm_infos, scm_list_1 (scm_info)));

  }

  scm_dynwind_end ();

  return scm_infos;
}

void
ggi_repository_init (void)
{
    g_debug ("ggi_repository_init");

#ifndef SCM_MAGIC_SNARFER
#include "ggi-repository.x"
#endif
}
