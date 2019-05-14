#include "ggi-repository.h"

SCM_DEFINE (scmggi_repository_require, "repository-require", 1, 2, 0,
            (SCM scm_namespace, SCM scm_version, SCM scm_lazy_p),
            "")
{
  gchar *namespace;
  gchar *version = NULL;
  GError *error = NULL;
  GIRepositoryLoadFlags flags = 0;

  if (!SCM_UNBNDP (scm_lazy_p) && scm_is_true (scm_lazy_p))
    flags |= G_IREPOSITORY_LOAD_FLAG_LAZY;

  scm_dynwind_begin (0);

  if (!SCM_UNBNDP (scm_version))
    version = scm_to_locale_string (scm_version);

  scm_dynwind_free (version);

  if (scm_is_symbol (scm_namespace))
    scm_namespace = scm_symbol_to_string (scm_namespace);

  namespace = scm_to_locale_string (scm_namespace);
  scm_dynwind_free (namespace);

  g_irepository_require (NULL, namespace, version, flags, &error);

  scm_dynwind_end ();

  if (error != NULL)
    {
      scm_misc_error ("repository-require", error->message, SCM_EOL);
      g_error_free (error);
    }

  return SCM_UNSPECIFIED;
}

SCM_DEFINE (scmggi_repository_find_by_name, "repository-find-by-name", 2, 0, 0,
            (SCM scm_namespace, SCM scm_name),
            "")
{
  gchar *namespace;
  gchar *name;
  GIBaseInfo *info;

  //SCM scm_object;

  scm_dynwind_begin (0);

  if (scm_is_symbol (scm_namespace))
    scm_namespace = scm_symbol_to_string (scm_namespace);
  if (scm_is_symbol (scm_name))
    scm_name = scm_symbol_to_string (scm_name);

  // TODO: error if not string

  namespace = scm_to_locale_string (scm_namespace);
  scm_dynwind_free (namespace);
  name = scm_to_locale_string (scm_name);
  scm_dynwind_free (name);

  info = g_irepository_find_by_name (NULL, namespace, name);

  scm_dynwind_end ();

  if (info)
    {
      return scm_from_pointer (info, NULL);
    }

  return SCM_BOOL_F;
}

void
scmggi_repository_init (void)
{
  g_debug ("scmgg_irepository.init");

  #ifndef SCM_MAGIC_SNARFER
  #include "ggi-repository.x"
  #endif
}
