
/* -*- Mode: C; c-basic-offset: 4 -*-
 * vim: tabstop=4 shiftwidth=4 expandtab
 */

#include "ggi-argument.h"

#include "ggi-value.h"
#include "ggi-basic-types.h"
#include "gi-info.h"

#include "gtype.h"
#include "gvalue.h"


#include <girffi.h>
#include <string.h>
#include <time.h>


gboolean
ggi_argument_to_gssize (GIArgument *arg_in,
                        GITypeTag   type_tag,
                        gssize     *gssize_out)
{
  switch (type_tag)
    {
    case GI_TYPE_TAG_INT8:
      *gssize_out = arg_in->v_int8;
      return TRUE;
    case GI_TYPE_TAG_UINT8:
      *gssize_out = arg_in->v_uint8;
      return TRUE;
    case GI_TYPE_TAG_INT16:
      *gssize_out = arg_in->v_int16;
      return TRUE;
    case GI_TYPE_TAG_UINT16:
      *gssize_out = arg_in->v_uint16;
      return TRUE;
    case GI_TYPE_TAG_INT32:
      *gssize_out = arg_in->v_int32;
      return TRUE;
    case GI_TYPE_TAG_UINT32:
      *gssize_out = arg_in->v_uint32;
      return TRUE;
    case GI_TYPE_TAG_INT64:
      if (arg_in->v_int64 > G_MAXSSIZE || arg_in->v_int64 < G_MINSSIZE)
        {
          scm_misc_error ("ggi_argument_to_gssize",
                          "Unable to marshal ~a to gssize",
                          scm_from_locale_string (g_type_tag_to_string (type_tag)));
          return FALSE;
        }
      *gssize_out = (gssize) arg_in->v_int64;
      return TRUE;
    case GI_TYPE_TAG_UINT64:
      if (arg_in->v_uint64 > G_MAXSSIZE || arg_in->v_uint64 < G_MINSSIZE)
        {
          scm_misc_error ("ggi_argument_to_gssize",
                          "Unable to marshal ~a to gssize",
                          scm_from_locale_string (g_type_tag_to_string (type_tag)));
          return FALSE;
        }
      *gssize_out = (gssize) arg_in->v_uint64;
      return TRUE;
    default:
      scm_misc_error ("ggi_argument_to_gssize",
                      "Unable to marshal ~a to gssize",
                      scm_from_locale_string (g_type_tag_to_string (type_tag)));
      return FALSE;
    }
}

static GITypeTag
_ggi_get_storage_type (GITypeInfo *type_info)
{
  GITypeTag type_tag = g_type_info_get_tag (type_info);

  if (type_tag == GI_TYPE_TAG_INTERFACE)
    {
      GIBaseInfo *interface = g_type_info_get_interface (type_info);

      switch (g_base_info_get_type (interface))
        {
        case GI_INFO_TYPE_ENUM:
        case GI_INFO_TYPE_FLAGS:
          type_tag = g_enum_info_get_storage_type ((GIEnumInfo *) interface);
          break;
        default:
          // FIXME: other types?
          break;
        }

      g_base_info_unref (interface);
    }

  return type_tag;
}

void
_ggi_hash_pointer_to_arg (GIArgument *arg,
                          GITypeInfo *type_info)
{
  GITypeTag type_tag = _ggi_get_storage_type (type_info);

  switch (type_tag)
    {
    case GI_TYPE_TAG_INT8:
      arg->v_int8 = (gint8) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_INT16:
      arg->v_int16 = (gint16) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_INT32:
      arg->v_int16 = (gint32) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_UINT8:
      arg->v_int16 = (guint8) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_UINT16:
      arg->v_int16 = (guint16) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_UINT32:
      arg->v_int16 = (guint32) GPOINTER_TO_INT (arg->v_pointer);
      break;
    case GI_TYPE_TAG_GTYPE:
      arg->v_size = GPOINTER_TO_SIZE (arg->v_pointer);
      break;
    case GI_TYPE_TAG_UTF8:
    case GI_TYPE_TAG_FILENAME:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_ARRAY:
      break;
    default:
      g_critical ("Unsupport type %s", g_type_tag_to_string (type_tag));
    }
}

gpointer
_ggi_arg_to_hash_pointer (const GIArgument *arg,
                          GITypeInfo       *type_info)
{
  GITypeTag type_tag = _ggi_get_storage_type (type_info);

  switch (type_tag)
    {
    case GI_TYPE_TAG_INT8:
      return GINT_TO_POINTER (arg->v_int8);
    case GI_TYPE_TAG_UINT8:
      return GINT_TO_POINTER (arg->v_uint8);
    case GI_TYPE_TAG_INT16:
      return GINT_TO_POINTER (arg->v_int16);
    case GI_TYPE_TAG_UINT16:
      return GINT_TO_POINTER (arg->v_uint16);
    case GI_TYPE_TAG_INT32:
      return GINT_TO_POINTER (arg->v_int32);
    case GI_TYPE_TAG_UINT32:
      return GINT_TO_POINTER (arg->v_uint32);
    case GI_TYPE_TAG_GTYPE:
      return GSIZE_TO_POINTER (arg->v_size);
    case GI_TYPE_TAG_UTF8:
    case GI_TYPE_TAG_FILENAME:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_ARRAY:
      return arg->v_pointer;
    default:
      g_critical ("Unsupported type %s", g_type_tag_to_string(type_tag));
      return arg->v_pointer;
    }
}

gssize
_ggi_argument_array_length_marshal (gsize length_arg_index,
                                    void *user_data1,
                                    void *user_data2)
{
  GIArgInfo length_arg_info;
  GITypeInfo length_type_info;
  GIArgument length_arg;
  gssize array_len = -1;
  GValue *values = (GValue *) user_data1;
  GICallableInfo *callable_info = (GICallableInfo *) user_data2;

  g_callable_info_load_arg (callable_info, (gint) length_arg_index, &length_arg_info);
  g_arg_info_load_type (&length_arg_info, &length_type_info);

  length_arg = _ggi_argument_from_g_value (&(values[length_arg_index]),
                                           &length_type_info);

  if (!ggi_argument_to_gssize (&length_arg,
                               g_type_info_get_tag (&length_type_info),
                               &array_len))
    return -1;

  return array_len;
}

GArray *
_ggi_argument_to_array (GIArgument              *arg,
                       GGIArgArrayLengthPolicy  array_length_policy,
                       void                    *user_data1,
                       void                    *user_data2,
                       GITypeInfo              *type_info,
                       gboolean                *out_free_array)
{
  GITypeInfo *item_type_info;
  gboolean is_zero_terminated;
  gsize item_size;
  gssize length;
  GArray *g_array;

  g_return_val_if_fail (g_type_info_get_tag (type_info) == GI_TYPE_TAG_ARRAY, NULL);

  if (arg->v_pointer == NULL)
    {
      return NULL;
    }

  switch (g_type_info_get_array_type (type_info))
    {
    case GI_ARRAY_TYPE_C:
      is_zero_terminated = g_type_info_is_zero_terminated (type_info);
      item_type_info = g_type_info_get_param_type (type_info, 0);

      item_size = _scmgi_c_g_type_info_size (item_type_info);

      g_base_info_unref ((GIBaseInfo *) item_type_info);

      if (is_zero_terminated)
        {
          length = g_strv_length (arg->v_pointer);
        }
      else
        {
          length = g_type_info_get_array_fixed_size (type_info);
          if (length < 0)
            {
              gint length_arg_pos;

              if (G_UNLIKELY (array_length_policy == NULL))
                {
                  g_critical ("Unable to determine array_length for %p", arg->v_pointer);
                  g_array = g_array_new (is_zero_terminated, FALSE, (guint) item_size);
                  *out_free_array = TRUE;
                  return g_array;
                }

              length_arg_pos = g_type_info_get_array_length (type_info);
              g_assert (length_arg_pos >= 0);

              length = array_length_policy (length_arg_pos, user_data1, user_data2);
              if (length < 0)
                return NULL;
            }
        }
      g_assert (length >= 0);

      g_array = g_array_new (is_zero_terminated, FALSE, (guint) item_size);

      g_free (g_array->data);
      g_array->data = arg->v_pointer;
      g_array->len = (guint) length;
      *out_free_array = TRUE;
      break;
    case GI_ARRAY_TYPE_ARRAY:
    case GI_ARRAY_TYPE_BYTE_ARRAY:
      g_array = arg->v_pointer;
      *out_free_array = FALSE;
      break;
    case GI_ARRAY_TYPE_PTR_ARRAY:
      {
        GPtrArray *ptr_array = (GPtrArray *) arg->v_pointer;
        g_array = g_array_sized_new (FALSE,
                                     FALSE,
                                     sizeof (gpointer),
                                     ptr_array->len);
        g_array->data = (char *) ptr_array->pdata;
        g_array->len = ptr_array->len;
        *out_free_array = TRUE;
        break;
      }
    default:
      g_critical ("Unexpected array type %u", g_type_info_get_array_type (type_info));
      g_array = NULL;
      break;
    }

  return g_array;
}

// TODO: implement :D
GIArgument
_ggi_argument_from_object (SCM         scm_obj,
                           GITypeInfo *type_info,
                           GITransfer  transfer)
{
  g_debug ("_ggi_argument_from_object");

  GIArgument arg;
  GITypeTag type_tag;
  gpointer cleanup_data = NULL;

  memset (&arg, 0, sizeof (GIArgument));
  type_tag = g_type_info_get_tag (type_info);

  switch (type_tag)
    {
    case GI_TYPE_TAG_ARRAY:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_GLIST:
    case GI_TYPE_TAG_GSLIST:
    case GI_TYPE_TAG_GHASH:
    case GI_TYPE_TAG_ERROR:
      g_assert_not_reached ();
    default:
      // ignores cleanup data for now
      ggi_marshal_from_scm_basic_type (scm_obj, &arg, type_tag, transfer, &cleanup_data);
    }

  return arg;
}

SCM
_ggi_argument_to_object (GIArgument *arg,
                         GITypeInfo *type_info,
                         GITransfer  transfer)
{
  g_debug ("_ggi_argument_to_object");

  GITypeTag type_tag;
  SCM scm_object = SCM_UNSPECIFIED;

  type_tag = g_type_info_get_tag (type_info);

  switch (type_tag)
    {
    case GI_TYPE_TAG_VOID:
      {
        if (g_type_info_is_pointer (type_info))
          {
            g_warn_if_fail (transfer == GI_TRANSFER_NOTHING);
            scm_from_pointer (arg->v_pointer, NULL);
          }
        break;
      }
    case GI_TYPE_TAG_ARRAY:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_GLIST:
    case GI_TYPE_TAG_GSLIST:
    case GI_TYPE_TAG_GHASH:
    case GI_TYPE_TAG_ERROR:
      g_assert_not_reached ();
    default:
      // ignores cleanup data for now
      scm_object = ggi_marshal_to_scm_basic_type (arg, type_tag, transfer);
    }

  return scm_object;
}
