#include "ggobject.h"

#include "ggi-argument.h"
#include "ggi-repository.h"

static SCM scmgi_base_info_class;

static SCM
scm_c_make_info (GIBaseInfo *info)
{
  SCM scm_class_variable;
  const gchar *repository_module;

  repository_module = "g-object introspection repository";

  switch (g_base_info_get_type (info))
    {
    case GI_INFO_TYPE_FUNCTION:
      scm_class_variable = scm_c_private_lookup (repository_module, "<function-info>");
      break;
    case GI_INFO_TYPE_CALLBACK:
      scm_class_variable = scm_c_private_lookup (repository_module, "<callback-info>");
      break;
    case GI_INFO_TYPE_STRUCT:
      scm_class_variable = scm_c_private_lookup (repository_module, "<struct-info>");
      break;
    case GI_INFO_TYPE_BOXED:
      scm_class_variable = scm_c_private_lookup (repository_module, "<boxed-info>");
      break;
    case GI_INFO_TYPE_ENUM:
      scm_class_variable = scm_c_private_lookup (repository_module, "<enum-info>");
      break;
    case GI_INFO_TYPE_FLAGS:
      scm_class_variable = scm_c_private_lookup (repository_module, "<flags-info>");
      break;
    case GI_INFO_TYPE_OBJECT:
      scm_class_variable = scm_c_private_lookup (repository_module, "<object-info>");
      break;
    case GI_INFO_TYPE_INTERFACE:
      scm_class_variable = scm_c_private_lookup (repository_module, "<interface-info>");
      break;
    case GI_INFO_TYPE_CONSTANT:
      scm_class_variable = scm_c_private_lookup (repository_module, "<constant-info>");
      break;
    case GI_INFO_TYPE_UNION:
      scm_class_variable = scm_c_private_lookup (repository_module, "<union-info>");
      break;
    case GI_INFO_TYPE_VALUE:
      scm_class_variable = scm_c_private_lookup (repository_module, "<value-info>");
      break;
    case GI_INFO_TYPE_SIGNAL:
      scm_class_variable = scm_c_private_lookup (repository_module, "<signal-info>");
      break;
    case GI_INFO_TYPE_VFUNC:
      scm_class_variable = scm_c_private_lookup (repository_module, "<v-func-info>");
      break;
    case GI_INFO_TYPE_PROPERTY:
      scm_class_variable = scm_c_private_lookup (repository_module, "<property-info>");
      break;
    case GI_INFO_TYPE_FIELD:
      scm_class_variable = scm_c_private_lookup (repository_module, "<field-info>");
      break;
    case GI_INFO_TYPE_ARG:
      scm_class_variable = scm_c_private_lookup (repository_module, "<arg-info>");
      break;
    case GI_INFO_TYPE_TYPE:
      scm_class_variable = scm_c_private_lookup (repository_module, "<type-info>");
      break;
    default:
      g_assert_not_reached ();
      return SCM_UNDEFINED;
    }

  return scm_make_foreign_object_1 (scm_variable_ref (scm_class_variable),
                                    (void *) info);
}

static GIBaseInfo *
scm_c_gi_base_info_get_info (SCM scm_info)
{
  return (GIBaseInfo *) scm_foreign_object_ref (scm_info, 0);
}

SCM_DEFINE (scmgi_base_info_display, "%info-display", 2, 0, 0,
            (SCM scm_info, SCM scm_port),
            "")
{
  const gchar *base_info_name;
  SCM scm_info_type;
  GIBaseInfo *info;

  info = scm_c_gi_base_info_get_info (scm_info);

  switch (g_base_info_get_type (info))
    {
    case GI_INFO_TYPE_FUNCTION:
      scm_info_type = scm_from_locale_string ("function");
      break;
    case GI_INFO_TYPE_CALLBACK:
      scm_info_type = scm_from_locale_string ("callback");
      break;
    case GI_INFO_TYPE_STRUCT:
      scm_info_type = scm_from_locale_string ("struct");
      break;
    case GI_INFO_TYPE_BOXED:
      scm_info_type = scm_from_locale_string ("boxed");
      break;
    case GI_INFO_TYPE_ENUM:
      scm_info_type = scm_from_locale_string ("enum");
      break;
    case GI_INFO_TYPE_FLAGS:
      scm_info_type = scm_from_locale_string ("flags");
      break;
    case GI_INFO_TYPE_OBJECT:
      scm_info_type = scm_from_locale_string ("object");
      break;
    case GI_INFO_TYPE_INTERFACE:
      scm_info_type = scm_from_locale_string ("interface");
      break;
    case GI_INFO_TYPE_CONSTANT:
      scm_info_type = scm_from_locale_string ("constant");
      break;
    case GI_INFO_TYPE_UNION:
      scm_info_type = scm_from_locale_string ("union");
      break;
    case GI_INFO_TYPE_VALUE:
      scm_info_type = scm_from_locale_string ("value");
      break;
    case GI_INFO_TYPE_SIGNAL:
      scm_info_type = scm_from_locale_string ("signal");
      break;
    case GI_INFO_TYPE_VFUNC:
      scm_info_type = scm_from_locale_string ("v-func");
      break;
    case GI_INFO_TYPE_PROPERTY:
      scm_info_type = scm_from_locale_string ("property");
      break;
    case GI_INFO_TYPE_FIELD:
      scm_info_type = scm_from_locale_string ("field");
      break;
    case GI_INFO_TYPE_ARG:
      scm_info_type = scm_from_locale_string ("arg");
      break;
    case GI_INFO_TYPE_TYPE:
      scm_info_type = scm_from_locale_string ("type");
      break;
    default:
      scm_info_type = scm_from_locale_string ("unknown");
    }

  base_info_name = g_base_info_get_name (info);

  scm_simple_format (scm_port,
                     scm_from_locale_string ("#<<~a-info> ~a>"),
                     scm_list_2 (scm_info_type,
                                 scm_from_locale_string (base_info_name)));


  return SCM_UNSPECIFIED;
}

static SCM
ggi_make_infos_list (SCM scm_info_class,
                     gint (*get_n_infos)(GIBaseInfo *),
                     GIBaseInfo * (*get_info)(GIBaseInfo *, gint))
{
  gint n_infos;
  gint infos_index;
  SCM scm_infos;

  n_infos = get_n_infos (scm_c_gi_base_info_get_info (scm_info_class));

  scm_infos = SCM_EOL;
  for (infos_index = 0; infos_index < n_infos; infos_index++)
    {
      GIBaseInfo *info;
      SCM scm_info;

      info = (GIBaseInfo *) get_info (scm_c_gi_base_info_get_info (scm_info_class),
                                      infos_index);

      g_assert (info != NULL);

      scm_info = scm_c_make_info (info);

      scm_infos = scm_append (scm_list_2 (scm_infos, scm_list_1 (scm_info)));
    }

  return scm_infos;
}



/*
  GIBaseInfo
*/

SCM_DEFINE (scmgi_base_info_get_namespace, "%info-get-namespace", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_locale_symbol (g_base_info_get_namespace (scm_c_gi_base_info_get_info (scm_info)));
}

SCM_DEFINE (scmgi_base_info_is_depcrecated, "%info-is-deprecated", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_bool (g_base_info_is_deprecated (scm_c_gi_base_info_get_info (scm_info)));
}

SCM_DEFINE (scmgi_base_info_get_name, "%info-get-name", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_locale_string (g_base_info_get_name (scm_c_gi_base_info_get_info (scm_info)));
}

/*
  GICallableInfo
 */

SCM_DEFINE (scmgi_callable_info_get_args, "%info-get-args", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return ggi_make_infos_list (scm_info, g_callable_info_get_n_args, g_callable_info_get_arg);
}

/*
  GIRepository
*/

SCM_DEFINE (scmgi_repository_require, "repository-require", 1, 2, 0,
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

SCM_DEFINE (scmgi_repository_find_by_name, "repository-find-by-name", 2, 0, 0,
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
      return scm_c_make_info (info);
    }

  return SCM_BOOL_F;
}


SCM_DEFINE (scmgi_arg_info_p, "%arg-info?", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_bool (GI_IS_ARG_INFO (scm_c_gi_base_info_get_info (scm_info)));
}

SCM_DEFINE (scmgi_callable_info_p, "%callable-info?", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_bool (GI_IS_CALLABLE_INFO (scm_c_gi_base_info_get_info (scm_info)));
}

/*
  GIConstantInfo
*/

SCM_DEFINE (ggi_constant_info_get_value, "%constant-value", 1, 0, 0,
            (SCM scm_info),
            "")
{
  GIConstantInfo *constant_info;
  GITypeInfo *type_info;
  GIArgument value = {0};
  SCM scm_value;

  constant_info = (GIConstantInfo *) scm_c_gi_base_info_get_info (scm_info);

  if (g_constant_info_get_value (constant_info, &value) < 0)
    {
      return SCM_UNDEFINED;
    }

  type_info = g_constant_info_get_type (constant_info);

  scm_value = ggi_arg_to_scm (type_info, GI_TRANSFER_NOTHING, value);
  g_constant_info_free_value (constant_info, &value);

  return scm_value;
}

/*
  GIRegisteredTypeInfo
 */

SCM_DEFINE (ggi_registered_type_info_p, "%registered-type-info?", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_bool (GI_IS_REGISTERED_TYPE_INFO (scm_c_gi_base_info_get_info (scm_info)));
}


/*
  GIObjectInfo
*/

SCM_DEFINE (scmgi_object_info_p, "%object-info?", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return scm_from_bool (GI_IS_OBJECT_INFO (scm_c_gi_base_info_get_info (scm_info)));
}

SCM_DEFINE (ggi_object_info_get_methods, "%object-info-get-methods", 1, 0, 0,
            (SCM scm_info),
            "")
{
  return ggi_make_infos_list (scm_info, g_object_info_get_n_methods, g_object_info_get_method);
}

static void
finalize_info (SCM scm_info)
{
  g_debug ("finalize");

  GIBaseInfo *info;

  info = scm_c_gi_base_info_get_info (scm_info);

  g_base_info_unref (info);
}

void
scmgi_repository_init (void)
{

  g_debug ("scmg_irepository_init");

#ifndef SCM_MAGIC_SNARFER
#include "ggi-repository.x"
#endif

  scm_t_struct_finalize info_finalizer;
  info_finalizer = finalize_info;

  scmgi_base_info_class = scm_make_foreign_object_type (scm_from_utf8_symbol ("<info>"),
                                                        scm_list_1 (scm_from_utf8_symbol ("info")),
                                                        info_finalizer);
  scm_c_define ("<info>", scmgi_base_info_class);
}
