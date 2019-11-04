/* -*- Mode: C; c-basic-offset: 4 -*-
 */

#include "gi-repository.h"
#include "gi-info.h"

SCM_DEFINE (scm_gi_repository_require, "gi-repository-require", 1, 2, 0,
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

SCM_DEFINE (scm_gi_repository_get_infos, "gi-repository-get-infos", 1, 0, 0,
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

    scm_info = scm_c_gi_make_info (info);

    scm_infos = scm_append (scm_list_2 (scm_infos, scm_list_1 (scm_info)));

  }

  scm_dynwind_end ();

  return scm_infos;
}

SCM_DEFINE (scm_gi_repository_find_by_name, "gi-repository-find-by-name", 2, 0, 0,
            (SCM scm_namespace, SCM scm_name),
            "")
{
    GIBaseInfo *info;
    char *namespace;
    char *name;

    SCM scm_info;

    scm_dynwind_begin (0);

    scm_namespace = scm_symbol_to_string (scm_namespace);
    namespace = scm_to_locale_string (scm_namespace);
    scm_dynwind_free (namespace);

    scm_name = scm_symbol_to_string (scm_name);
    name = scm_to_locale_string (scm_name);
    scm_dynwind_free (name);

    info = g_irepository_find_by_name (NULL, namespace, name);
    scm_info = scm_c_gi_make_info (info);

    scm_dynwind_end ();

    return scm_info;
}

void
scm_c_gi_repository_init (void)
{
    g_debug ("scm_c_gi_repository_init");

#ifndef SCM_MAGIC_SNARFER
#include "gi-repository.x"
#endif
}
