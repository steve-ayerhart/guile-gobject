#include "gi-info.h"

#include "ggi-cache.h"
#include "utils.h"

#include <glib.h>

static SCM scm_gi_base_info_class;
static SCM scm_gi_callable_info_class;
static SCM scm_gi_function_info_class;
static SCM scm_gi_callback_info_class;
static SCM scm_gi_signal_info_class;
static SCM scm_gi_vfunc_info_class;
static SCM scm_gi_registered_type_info_class;
static SCM scm_gi_enum_info_class;
static SCM scm_gi_struct_info_class;
static SCM scm_gi_union_info_class;
static SCM scm_gi_object_info_class;
static SCM scm_gi_interface_info_class;
static SCM scm_gi_arg_info_class;
static SCM scm_gi_constant_info_class;
static SCM scm_gi_property_info_class;
static SCM scm_gi_type_info_class;
static SCM scm_gi_value_info_class;
static SCM scm_gi_field_info_class;

static SCM scm_gtype_name_to_scheme_name;

gsize
_scmgi_c_g_type_tag_size (GITypeTag type_tag)
{
  gsize size = 0;

  switch (type_tag) {
  case GI_TYPE_TAG_BOOLEAN:
    size = sizeof (gboolean);
    break;
  case GI_TYPE_TAG_INT8:
  case GI_TYPE_TAG_UINT8:
    size = sizeof (gint8);
    break;
  case GI_TYPE_TAG_INT16:
  case GI_TYPE_TAG_UINT16:
    size = sizeof (gint16);
    break;
  case GI_TYPE_TAG_INT32:
  case GI_TYPE_TAG_UINT32:
    size = sizeof (gint32);
    break;
  case GI_TYPE_TAG_INT64:
  case GI_TYPE_TAG_UINT64:
    size = sizeof (gint64);
    break;
  case GI_TYPE_TAG_FLOAT:
    size = sizeof (gfloat);
    break;
  case GI_TYPE_TAG_DOUBLE:
    size = sizeof (gdouble);
    break;
  case GI_TYPE_TAG_GTYPE:
    size = sizeof (GType);
    break;
  case GI_TYPE_TAG_UNICHAR:
    size = sizeof (gunichar);
    break;
  case GI_TYPE_TAG_VOID:
  case GI_TYPE_TAG_UTF8:
  case GI_TYPE_TAG_FILENAME:
  case GI_TYPE_TAG_ARRAY:
  case GI_TYPE_TAG_INTERFACE:
  case GI_TYPE_TAG_GLIST:
  case GI_TYPE_TAG_GSLIST:
  case GI_TYPE_TAG_GHASH:
  case GI_TYPE_TAG_ERROR:
    scm_misc_error ("_scmgi_c_g_type_tag_size",
                    "Unable to know the size (assuming ~a is not a pointer)",
                    scm_from_locale_string (g_type_tag_to_string (type_tag)));
    break;
  default:
    break;
  }

  return size; 
}



gsize
_scmgi_c_g_type_info_size (GITypeInfo *type_info)
{
  gsize size = 0;
  GITypeTag type_tag;

  type_tag = g_type_info_get_tag (type_info);
  switch (type_tag) {
  case GI_TYPE_TAG_BOOLEAN:
  case GI_TYPE_TAG_INT8:
  case GI_TYPE_TAG_UINT8:
  case GI_TYPE_TAG_INT16:
  case GI_TYPE_TAG_UINT16:
  case GI_TYPE_TAG_INT32:
  case GI_TYPE_TAG_UINT32:
  case GI_TYPE_TAG_INT64:
  case GI_TYPE_TAG_UINT64:
  case GI_TYPE_TAG_FLOAT:
  case GI_TYPE_TAG_DOUBLE:
  case GI_TYPE_TAG_GTYPE:
  case GI_TYPE_TAG_UNICHAR:
    size = _scmgi_c_g_type_tag_size (type_tag);
    g_assert (size > 0);
    break;
  case GI_TYPE_TAG_INTERFACE:
    {
      GIBaseInfo *info;
      GIInfoType info_type;

      info = g_type_info_get_interface (type_info);
      info_type = g_base_info_get_type (info);

      switch (info_type) {
      case GI_INFO_TYPE_STRUCT:
        if (g_type_info_is_pointer (type_info)) {
          size = sizeof (gpointer);
        } else {
          size = g_struct_info_get_size ( (GIStructInfo *) info);
        }
        break;
      case GI_INFO_TYPE_UNION:
        if (g_type_info_is_pointer (type_info)) {
          size = sizeof (gpointer);
        } else {
          size = g_union_info_get_size ( (GIUnionInfo *) info);
        }
        break;
      case GI_INFO_TYPE_ENUM:
      case GI_INFO_TYPE_FLAGS:
        if (g_type_info_is_pointer (type_info)) {
          size = sizeof (gpointer);
        } else {
          GITypeTag enum_type_tag;

          enum_type_tag = g_enum_info_get_storage_type ( (GIEnumInfo *) info);
          size = _scmgi_c_g_type_tag_size (enum_type_tag);
        }
        break;
      case GI_INFO_TYPE_BOXED:
      case GI_INFO_TYPE_OBJECT:
      case GI_INFO_TYPE_INTERFACE:
      case GI_INFO_TYPE_CALLBACK:
        size = sizeof (gpointer);
        break;
      case GI_INFO_TYPE_VFUNC:
      case GI_INFO_TYPE_INVALID:
      case GI_INFO_TYPE_FUNCTION:
      case GI_INFO_TYPE_CONSTANT:
      case GI_INFO_TYPE_VALUE:
      case GI_INFO_TYPE_SIGNAL:
      case GI_INFO_TYPE_PROPERTY:
      case GI_INFO_TYPE_FIELD:
      case GI_INFO_TYPE_ARG:
      case GI_INFO_TYPE_TYPE:
      case GI_INFO_TYPE_UNRESOLVED:
      default:
        g_assert_not_reached();
        break;
      }

      g_base_info_unref (info);
      break;
    }
  case GI_TYPE_TAG_ARRAY:
  case GI_TYPE_TAG_VOID:
  case GI_TYPE_TAG_UTF8:
  case GI_TYPE_TAG_FILENAME:
  case GI_TYPE_TAG_GLIST:
  case GI_TYPE_TAG_GSLIST:
  case GI_TYPE_TAG_GHASH:
  case GI_TYPE_TAG_ERROR:
    size = sizeof (gpointer);
    break;
  default:
    break;
  }

  return size;
}

void
_scm_finalize_callable_cache (void *cache)
{
  g_debug ("_scm_finalize_callable_cache: %s", ((GGICallableCache *) cache)->name);
}

SCM_DEFINE (scm_gi_finalizer, "%gi-base-info-finalizer", 1, 0, 0,
            (SCM scm_info),
            "")
{
  g_debug ("finalizing obj");

  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_info);

  GIBaseInfo *info;

  info = (GIBaseInfo *) scm_foreign_object_ref (scm_info, 0);

  g_base_info_unref (info);

  return SCM_UNSPECIFIED;
}


/*
 * Make a list from the common GI API pattern of having a function which
 * returns a count and an indexed GIBaseInfo in the range of 0 to count
 */

SCM
_scm_c_gi_make_infos_list (SCM scm_info_class,
                           gint (*get_n_infos)(GIBaseInfo*),
                           GIBaseInfo* (*get_info)(GIBaseInfo*, gint))
{
  gint n_infos;
  SCM scm_infos;
  gint infos_index;

  n_infos = get_n_infos ((GIBaseInfo *) scm_foreign_object_ref (scm_info_class, 0));

  scm_infos = SCM_EOL;

  for (infos_index = 0; infos_index < n_infos; infos_index++) {
    GIBaseInfo *info;
    SCM scm_info;

    info = (GIBaseInfo *) get_info ((GIBaseInfo *) scm_foreign_object_ref (scm_info_class, 0),
                                    infos_index);
    g_assert (info != NULL);

    scm_info = scm_c_gi_make_info (info);

    scm_infos = scm_append (scm_list_2 (scm_infos, scm_list_1 (scm_info)));
  }

  return scm_infos;
}

SCM
scm_c_gi_make_info (GIBaseInfo *info)
{
  GIInfoType info_type;
  SCM scm_info;

  info_type = g_base_info_get_type (info);

  switch (info_type)
    {
    case GI_INFO_TYPE_INVALID:
      g_critical ("Invalid info type");
      return SCM_UNSPECIFIED;
    case GI_INFO_TYPE_FUNCTION:
      scm_info= scm_gi_function_info_class;
      break;
    case GI_INFO_TYPE_CALLBACK:
      scm_info = scm_gi_callback_info_class;
      break;
    case GI_INFO_TYPE_STRUCT:
    case GI_INFO_TYPE_BOXED:
      scm_info = scm_gi_struct_info_class;
      break;
    case GI_INFO_TYPE_ENUM:
    case GI_INFO_TYPE_FLAGS:
      scm_info = scm_gi_enum_info_class;
      break;
    case GI_INFO_TYPE_OBJECT:
      scm_info = scm_gi_object_info_class;
      break;
    case GI_INFO_TYPE_INTERFACE:
      scm_info = scm_gi_interface_info_class;
      break;
    case GI_INFO_TYPE_CONSTANT:
      scm_info = scm_gi_constant_info_class;
      break;
    case GI_INFO_TYPE_UNION:
      scm_info = scm_gi_union_info_class;
      break;
    case GI_INFO_TYPE_VALUE:
      scm_info = scm_gi_value_info_class;
      break;
    case GI_INFO_TYPE_SIGNAL:
      scm_info = scm_gi_signal_info_class;
      break;
    case GI_INFO_TYPE_VFUNC:
      scm_info = scm_gi_vfunc_info_class;
      break;
    case GI_INFO_TYPE_PROPERTY:
      scm_info = scm_gi_property_info_class;
      break;
    case GI_INFO_TYPE_FIELD:
      scm_info = scm_gi_field_info_class;
      break;
    case GI_INFO_TYPE_ARG:
      scm_info = scm_gi_arg_info_class;
      break;
    case GI_INFO_TYPE_TYPE:
      scm_info = scm_gi_type_info_class;
      break;
    default:
      g_assert_not_reached ();
      return SCM_UNSPECIFIED;
    }

  return scm_make_foreign_object_1 (scm_info, (void *) info);
}

GIBaseInfo *
scm_object_get_gi_info (SCM scm_object)
{
  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_object);

  return (GIBaseInfo *) scm_foreign_object_ref (scm_object, 0);
}


SCM_DEFINE (scm_g_base_info_get_type, "%g-base-info-get-type", 1, 0, 0,
            (SCM scm_base_info),
            "")
{
  return scm_from_uint (g_base_info_get_type (scm_object_get_gi_info (scm_base_info)));
}

SCM_DEFINE (scm_g_base_info_get_name, "%g-base-info-get-name", 1, 0, 0,
            (SCM scm_base_info),
            "")
{
  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_base_info);

  GIBaseInfo *base_info;
  const gchar *name;

  base_info = scm_object_get_gi_info (scm_base_info);

  if (g_base_info_get_type (base_info) == GI_INFO_TYPE_TYPE)
    {
      name = "type_type_instance";
    }
  else
    {
      name = g_base_info_get_name (base_info);
    }

  // TODO: need to escape any names?

  return scm_from_locale_string (name);
}

SCM_DEFINE (scm_g_base_info_get_namespace, "%g-base-info-get-namespace", 1, 0, 0,
            (SCM scm_base_info),
            "")
{
  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_base_info);

  GIBaseInfo *base_info;
  const gchar *namespace;

  base_info = scm_object_get_gi_info (scm_base_info);
  namespace = g_base_info_get_name (base_info);

  return scm_from_locale_string (namespace);
}

SCM_DEFINE (scm_g_base_info_is_deprecated, "%g-base-info-is-deprecated", 1, 0, 0,
            (SCM scm_base_info),
            "")
{
  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_base_info);

  GIBaseInfo *base_info;

  base_info = scm_object_get_gi_info (scm_base_info);
  return scm_from_bool (g_base_info_is_deprecated (base_info));
}

SCM_DEFINE (scm_g_base_info_get_attributes, "%g-base-info-get-attributes", 1, 0, 0,
            (SCM scm_base_info),
            "")
{
  scm_assert_foreign_object_type (scm_gi_base_info_class, scm_base_info);

  GIBaseInfo *base_info;
  GIAttributeIter a_iter = { 0, };
  //  SCM scm_attributes;
  char *name;
  char *value;

  base_info = scm_object_get_gi_info (scm_base_info);

  while (g_base_info_iterate_attributes (base_info, &a_iter, &name, &value))
    {
      g_debug ("heey: %s -> %s", name, value);
    }
  /*
    scm_name = scm_symbol_to_string (scm_name);
    name = scm_to_locale_string (scm_name);

    value = g_base_info_get_attribute (base_info, name);
    g_free (name);

    if (value == NULL)
    return SCM_UNSPECIFIED;

    return scm_from_locale_symbol (value);
  */
  return SCM_UNSPECIFIED;
}

// GIRegisteredInfo

SCM_DEFINE (scm_g_registered_type_info_get_type_name, "%g-registered-type-info-get-type-name", 1, 0, 0,
            (SCM scm_registered_type_info),
            "")
{
  GIRegisteredTypeInfo *registered_type_info;

  registered_type_info = (GIRegisteredTypeInfo *) scm_object_get_gi_info (scm_registered_type_info);

  return scm_from_locale_string (g_registered_type_info_get_type_name (registered_type_info));
}

SCM_DEFINE (scm_g_registered_type_info_get_gtype, "%g-registered-type-info-get-gtype", 1, 0, 0,
            (SCM scm_registered_type_info),
            ""
            )
{
  GIRegisteredTypeInfo *registered_type_info;
  GType gtype;

  registered_type_info = (GIRegisteredTypeInfo *) scm_object_get_gi_info (scm_registered_type_info);
  gtype = g_registered_type_info_get_g_type (registered_type_info);

  if (gtype == G_TYPE_INVALID)
    {
      return SCM_BOOL_F;
    }


  return scm_from_ulong (gtype);
}

// GIConstantInfo

SCM_DEFINE (scm_gi_constant_info_get_value, "%gi-constant-info-get-value", 1, 0, 0,
            (SCM scm_constant_info),
            "")
{
  GITypeInfo *type_info;
  GIConstantInfo *constant_info;
  GIArgument value = {0};
  SCM scm_value;

  constant_info = (GIConstantInfo *) scm_object_get_gi_info (scm_constant_info);

  if (g_constant_info_get_value (constant_info, &value) < 0) {
    return SCM_UNDEFINED;
  }

  type_info = g_constant_info_get_type (constant_info);

  scm_value = gi_arg_to_scm (type_info, GI_TRANSFER_NOTHING, value);
  g_constant_info_free_value (constant_info, &value);

  scm_remember_upto_here_1 (constant_info);

  return scm_value;
}

/*
 * GIObjectInfo
 SCM_DEFINE (scm_gi_object_info_get_methods, "%gi-object-info-get-methods", 1, 0, 0,
 (SCM scm_object_info),
 ""
 )
 {
 return ggi_make_infos_list (scm_object_info, g_object_info_get_n_methods, g_object_info_get_method);
 }
*/

/*
 * GIEnumInfo
 SCM_DEFINE (scm_gi_enum_info_get_methods, "%gi-enum-info-get-methods", 1, 0, 0,
 (SCM scm_enum_info),
 ""
 )
 {
 return ggi_make_infos_list (scm_enum_info, g_enum_info_get_n_methods, g_enum_info_get_method);
 }
*/


/*
 * GIUnionInfo

 SCM_DEFINE (scm_gi_union_info_get_methods, "%gi-union-info-get-methods", 1, 0, 0,
 (SCM scm_union_info),
 ""
 )
 {
 return ggi_make_infos_list (scm_union_info, g_union_info_get_n_methods, g_union_info_get_method);
 }
*/ 

/*
 * GICallableInfo

 SCM_DEFINE (scm_gi_callable_info_call, "%gi-callable-info-call", 3, 0, 0,
 (SCM scm_callable_info, SCM scm_args, SCM scm_kwargs),
 ""
 )
 {
 return _wrap_g_callable_info_invoke (scm_callable_info, scm_args, scm_kwargs);
 }
*/

/*
 * GIArgInfo

 #define DIRECTION_IN_SYMBOL "gi-direction-in"
 #define DIRECTION_OUT_SYMBOL "gi-direction-out"
 #define DIRECTION_INOUT_SYMBOL "gi-direction-inout"

 #define SCOPE_TYPE_INVALID_SYMBOL "gi-scope-type-invalid"
 #define SCOPE_TYPE_CALL_SYMBOL "gi-scope-type-call"
 #define SCOPE_TYPE_ASYNC_SYMBOL "gi-scope-type-async"
 #define SCOPE_TYPE_NOTIFIED_SYMBOL "gi-scope-type-notified"


 SCM_DEFINE (scm_g_arg_info_get_direction, "%gi-arg-info-get-direction", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_int (g_arg_info_get_direction (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_is_return_value, "%gi-arg-info-is-return-value?", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_bool (g_arg_info_is_return_value (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_is_optional, "%gi-arg-info-is-optional?", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_bool (g_arg_info_is_optional (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_may_be_null, "%gi-arg-info-may-be-null?", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_bool (g_arg_info_may_be_null (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_get_ownership_transfer, "%gi-arg-info-get-ownership-transfer", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_int (g_arg_info_get_ownership_transfer (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_get_scope, "%gi-arg-info-get-scope", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_int (g_arg_info_get_scope (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_get_closure, "%gi-arg-info-get-closure", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_int (g_arg_info_get_closure (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_get_destroy, "%gi-arg-info-get-destroy", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);

 return scm_from_int (g_arg_info_get_destroy (arg_info));
 }

 SCM_DEFINE (scm_g_arg_info_get_type, "%gi-arg-info-get-type", 1, 0, 0,
 (SCM scm_arg_info),
 ""
 )
 {
 GIArgInfo *arg_info;
 GITypeInfo *type_info;
 SCM scm_type;

 arg_info = (GIArgInfo *) scm_object_get_gi_info (scm_arg_info);
 type_info = g_arg_info_get_type (arg_info);

 if (type_info == NULL)
 scm_type = SCM_BOOL_F;
 else {
 scm_type = ggi_make_info (type_info);
 }

 scm_remember_upto_here_1 (type_info);

 return scm_type;
 }
*/

/*

  gsize
  _scm_g_class_tag_size (GITypeTag type_tag)
  {
  gsize size = 0;

  switch (type_tag) {
  case GI_TYPE_TAG_BOOLEAN:
  size = sizeof (gboolean);
  break;
  case GI_TYPE_TAG_INT8:
  case GI_TYPE_TAG_UINT8:
  size = sizeof (gint8);
  break;
  case GI_TYPE_TAG_INT16:
  case GI_TYPE_TAG_UINT16:
  size = sizeof (gint16);
  break;
  case GI_TYPE_TAG_INT32:
  case GI_TYPE_TAG_UINT32:
  size = sizeof (gint32);
  break;
  case GI_TYPE_TAG_INT64:
  case GI_TYPE_TAG_UINT64:
  size = sizeof (gint64);
  break;
  case GI_TYPE_TAG_FLOAT:
  size = sizeof (gfloat);
  break;
  case GI_TYPE_TAG_DOUBLE:
  size = sizeof (gdouble);
  break;
  case GI_TYPE_TAG_GTYPE:
  size = sizeof (GType);
  break;
  case GI_TYPE_TAG_UNICHAR:
  size = sizeof (gunichar);
  break;
  case GI_TYPE_TAG_VOID:
  case GI_TYPE_TAG_UTF8:
  case GI_TYPE_TAG_FILENAME:
  case GI_TYPE_TAG_ARRAY:
  case GI_TYPE_TAG_INTERFACE:
  case GI_TYPE_TAG_GLIST:
  case GI_TYPE_TAG_GSLIST:
  case GI_TYPE_TAG_GHASH:
  case GI_TYPE_TAG_ERROR:
  scm_misc_error("gtype size error",
  "Unable to know the size (assuming ~s is not a pointer)",
  scm_from_locale_string (g_type_tag_to_string (type_tag)));
  break;
  default:
  break;
  }

  return size;
  }

  gsize
  _scm_g_class_info_size (GITypeInfo *type_info)
  {
  gsize size = 0;

  GITypeTag type_tag;

  type_tag = g_type_info_get_tag (type_info);
  switch (type_tag) {
  case GI_TYPE_TAG_BOOLEAN:
  case GI_TYPE_TAG_INT8:
  case GI_TYPE_TAG_UINT8:
  case GI_TYPE_TAG_INT16:
  case GI_TYPE_TAG_UINT16:
  case GI_TYPE_TAG_INT32:
  case GI_TYPE_TAG_UINT32:
  case GI_TYPE_TAG_INT64:
  case GI_TYPE_TAG_UINT64:
  case GI_TYPE_TAG_FLOAT:
  case GI_TYPE_TAG_DOUBLE:
  case GI_TYPE_TAG_GTYPE:
  case GI_TYPE_TAG_UNICHAR:
  size = _scm_g_class_tag_size (type_tag);
  g_assert (size > 0);
  break;
  case GI_TYPE_TAG_INTERFACE:
  {
  GIBaseInfo *info;
  GIInfoType info_type;

  info = g_type_info_get_interface (type_info);
  info_type = g_base_info_get_type (info);

  switch (info_type) {
  case GI_INFO_TYPE_STRUCT:
  if (g_type_info_is_pointer (type_info)) {
  size = sizeof (gpointer);
  } else {
  size = g_struct_info_get_size ( (GIStructInfo *) info);
  }
  break;
  case GI_INFO_TYPE_UNION:
  if (g_type_info_is_pointer (type_info)) {
  size = sizeof (gpointer);
  } else {
  size = g_union_info_get_size ( (GIUnionInfo *) info);
  }
  break;
  case GI_INFO_TYPE_ENUM:
  case GI_INFO_TYPE_FLAGS:
  if (g_type_info_is_pointer (type_info)) {
  size = sizeof (gpointer);
  } else {
  GITypeTag enum_type_tag;

  enum_type_tag = g_enum_info_get_storage_type ( (GIEnumInfo *) info);
  size = _scm_g_type_tag_size (enum_class_tag);
  }
  break;
  case GI_INFO_TYPE_BOXED:
  case GI_INFO_TYPE_OBJECT:
  case GI_INFO_TYPE_INTERFACE:
  case GI_INFO_TYPE_CALLBACK:
  size = sizeof (gpointer);
  break;
  case GI_INFO_TYPE_VFUNC:
  case GI_INFO_TYPE_INVALID:
  case GI_INFO_TYPE_FUNCTION:
  case GI_INFO_TYPE_CONSTANT:
  case GI_INFO_TYPE_VALUE:
  case GI_INFO_TYPE_SIGNAL:
  case GI_INFO_TYPE_PROPERTY:
  case GI_INFO_TYPE_FIELD:
  case GI_INFO_TYPE_ARG:
  case GI_INFO_TYPE_TYPE:
  case GI_INFO_TYPE_UNRESOLVED:
  default:
  g_assert_not_reached();
  break;
  }

  g_base_info_unref (info);
  break;
  }
  case GI_TYPE_TAG_ARRAY:
  case GI_TYPE_TAG_VOID:
  case GI_TYPE_TAG_UTF8:
  case GI_TYPE_TAG_FILENAME:
  case GI_TYPE_TAG_GLIST:
  case GI_TYPE_TAG_GSLIST:
  case GI_TYPE_TAG_GHASH:
  case GI_TYPE_TAG_ERROR:
  size = sizeof (gpointer);
  break;
  default:
  break;
  }

  return size;
  }
*/

// stuff
static const char *
_safe_base_info_get_name (GIBaseInfo *info)
{
  if (g_base_info_get_type (info) == GI_INFO_TYPE_TYPE) {
    return "type_type_instance";
  } else {
    return g_base_info_get_name (info);
  }
}

gchar *
_scm_c_gi_base_info_get_fullname (GIBaseInfo *info)
{
  GIBaseInfo *container_info;
  gchar *fullname;

  container_info = g_base_info_get_container (info);
  if (container_info != NULL) {
    fullname = g_strdup_printf ("%s %s %s",
                                g_base_info_get_namespace (container_info),
                                _safe_base_info_get_name (container_info),
                                _safe_base_info_get_name (info));
  } else {
    fullname = g_strdup_printf ("%s %s",
                                g_base_info_get_namespace (info),
                                _safe_base_info_get_name (info));
  }

  if (fullname == NULL) {
    g_critical ("OOM");
  }

  return fullname;
}

SCM_DEFINE (scm_c_g_function_info_to_scm, "%gi-function-info->scm", 1, 0, 0,
            (SCM scm_function_info),
            "")
{
  GIFunctionInfo *function_info;
  GGIFunctionCache *function_cache;
  GGICallableCache *callable_cache;
  const char *function_name;

  SCM scm_callable_cache;
  SCM scm_function;

  function_info = (GIFunctionInfo *) scm_object_get_gi_info (scm_function_info);

  GI_IS_FUNCTION_INFO (function_info);

  function_cache = ggi_function_cache_new ((GICallableInfo *) function_info);

  callable_cache = (GGICallableCache *) function_cache;

  g_assert (function_cache != NULL);

  function_name = scm_c_gtype_name_to_scheme_name (callable_cache->name);

  scm_c_define_gsubr (function_name,
                      callable_cache->n_scm_required_args,
                      callable_cache->n_scm_args - callable_cache->n_scm_required_args,
                      0,
                      function_cache->wrapper);

  scm_function = scm_variable_ref (scm_c_lookup (function_name));

  scm_callable_cache = scm_from_pointer (callable_cache, _scm_finalize_callable_cache);
  scm_set_object_property_x (scm_function,
                             scm_from_locale_symbol ("callable-cache"),
                             scm_callable_cache);
  return scm_function;
}

/*
 * init
 */

void
scm_c_gi_info_early_init (void)
{
  g_debug ("scm_c_gi_info_early_init");

#ifndef SCM_MAGIC_SNARFER
#include "gi-info.x"
#endif
}

void
scm_c_gi_info_init (void)
{
  g_debug ("scm_c_gi_info_init");

  scm_gi_base_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup ("<gi-base-info>")));
  scm_gi_callable_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-callable-info>")));
  scm_gi_callback_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-callback-info>")));
  scm_gi_function_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-function-info>")));
  scm_gi_signal_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-signal-info>")));
  scm_gi_vfunc_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-vfunc-info>")));

  scm_gi_registered_type_info_class =
    scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-registered-type-info>")));
  scm_gi_enum_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-enum-info>")));
  scm_gi_interface_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-interface-info>")));
  scm_gi_object_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-object-info>")));
  scm_gi_struct_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-struct-info>")));
  scm_gi_union_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-union-info>")));

  scm_gi_arg_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-arg-info>")));
  scm_gi_constant_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-constant-info>")));
  scm_gi_field_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-field-info>")));
  scm_gi_property_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-property-info>")));
  scm_gi_type_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-type-info>")));
  scm_gi_value_info_class = scm_permanent_object (scm_variable_ref (scm_c_lookup("<gi-value-info>")));

  /*
    scm_c_define (DIRECTION_IN_SYMBOL, scm_from_int (GI_DIRECTION_IN));
    scm_c_define (DIRECTION_OUT_SYMBOL, scm_from_int (GI_DIRECTION_OUT));
    scm_c_define (DIRECTION_INOUT_SYMBOL, scm_from_int (GI_DIRECTION_INOUT));
    scm_c_define (SCOPE_TYPE_INVALID_SYMBOL, scm_from_int (GI_SCOPE_TYPE_INVALID));
    scm_c_define (SCOPE_TYPE_CALL_SYMBOL, scm_from_int (GI_SCOPE_TYPE_CALL));
    scm_c_define (SCOPE_TYPE_ASYNC_SYMBOL, scm_from_int (GI_SCOPE_TYPE_ASYNC));
    scm_c_define (SCOPE_TYPE_NOTIFIED_SYMBOL, scm_from_int (GI_SCOPE_TYPE_NOTIFIED));

    // TODO: ugh
    */
}
