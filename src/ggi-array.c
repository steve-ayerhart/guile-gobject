#include "ggi-array.h"

#include "gi-info.h"
#include "ggi-cache.h"
#include "ggi-basic-types.h"

static gboolean
gi_argument_to_gsize (GIArgument *arg_in,
                      gsize      *gsize_out,
                      GITypeTag   type_tag)
{
  switch (type_tag) {
  case GI_TYPE_TAG_INT8:
    *gsize_out = arg_in->v_int8;
    return TRUE;
  case GI_TYPE_TAG_UINT8:
    *gsize_out = arg_in->v_uint8;
    return TRUE;
  case GI_TYPE_TAG_INT16:
    *gsize_out = arg_in->v_int16;
    return TRUE;
  case GI_TYPE_TAG_UINT16:
    *gsize_out = arg_in->v_uint16;
    return TRUE;
  case GI_TYPE_TAG_INT32:
    *gsize_out = arg_in->v_int32;
    return TRUE;
  case GI_TYPE_TAG_UINT32:
    *gsize_out = arg_in->v_uint32;
    return TRUE;
  case GI_TYPE_TAG_INT64:
    if (arg_in->v_uint64 > G_MAXSIZE) {
      scm_misc_error ("gi_argument_to_gsize",
                      "Unable to marshal ~a to gsize",
                      scm_from_locale_string (g_type_tag_to_string(type_tag)));
      return FALSE;
    }
    *gsize_out = (gsize)arg_in->v_int64;
    return TRUE;
  case GI_TYPE_TAG_UINT64:
    if (arg_in->v_uint64 > G_MAXSIZE) {
      scm_misc_error ("gi_argument_to_gsize",
                      "Unable to marshal ~a to gsize",
                      scm_from_locale_string (g_type_tag_to_string(type_tag)));
      return FALSE;
    }
    *gsize_out = (gsize)arg_in->v_uint64;
    return TRUE;
  default:
    scm_misc_error ("gi_argument_to_gsize",
                    "Unable to marshal ~a to gsize",
                    scm_from_locale_string (g_type_tag_to_string(type_tag)));
    return FALSE;
  }
}

// GArray to SCM
static SCM
_ggi_marshal_to_scm_array (GGIInvokeState   *state,
                           GGICallableCache *callable_cache,
                           GGIArgCache      *arg_cache,
                           GIArgument       *arg,
                           gpointer         *cleanup_data)
{
  GArray *array_;
  GGISequenceCache *seq_cache = (GGISequenceCache *) arg_cache;
  GGIArgGArray *array_cache = (GGIArgGArray *) arg_cache;
  guint processed_items = 0;
  SCM scm_return_list;

  // GArrays make it easier to iterate over arrays
  // with different element sizes but requires that
  // we allocate a GArray if the argument was a C array
  if (array_cache->array_type == GI_ARRAY_TYPE_C)
    {
      gsize len;
      if (array_cache->fixed_size >= 0)
        {
          g_assert (arg->v_pointer != NULL);
          len = array_cache->fixed_size;
        }
      else if (array_cache->is_zero_terminated)
        {
          if (arg->v_pointer == NULL)
            {
              len = 0;
            }
          else if (seq_cache->item_cache->type_tag == GI_TYPE_TAG_UINT8)
            {
              len = strlen (arg->v_pointer);
            }
          else
            {
              len = g_strv_length ((gchar **) arg->v_pointer);
            }
        }
      else
        {
          GIArgument *len_arg = &state->args[array_cache->len_arg_index].arg_value;
          GGIArgCache *sub_cache = _ggi_callable_cache_get_arg (callable_cache,
                                                                (guint) array_cache->len_arg_index);

          if (!gi_argument_to_gsize (len_arg, &len, sub_cache->type_tag))
            {
              return NULL;
            }
        }

      array_ = g_array_new (FALSE, FALSE, (guint) array_cache->item_size);
      if (array_ == NULL)
        {
          // TODO: OOM

          if (arg_cache->transfer == GI_TRANSFER_EVERYTHING && arg->v_pointer != NULL)
            {
              g_free (arg->v_pointer);
            }

          return NULL;
        }

      if (array_->data != NULL)
        {
          g_free (array_->data);
        }
      array_->data = arg->v_pointer;
      array_->len = (guint) len;
    }
  else
    {
      array_ = arg->v_pointer;
    }

  if (seq_cache->item_cache->type_tag == GI_TYPE_TAG_UINT8)
    {
      if (arg->v_pointer == NULL)
        {
          scm_return_list = scm_from_utf8_string ("");
        }
      else
        {
          scm_return_list = scm_from_utf8_stringn (array_->data, array_->len);
        }
    }
  else
    {
      if (arg->v_pointer == NULL)
        {
          scm_return_list = SCM_EOL;
        }
      else
        {
          guint i;
          gsize item_size;
          GGIMarshalToScmFunc item_to_scm_marshaller;
          GGIArgCache *item_arg_cache;
          GPtrArray *item_cleanups;

          scm_return_list = SCM_EOL;

          item_cleanups = g_ptr_array_sized_new (array_->len);
          *cleanup_data = item_cleanups;

          item_arg_cache = seq_cache->item_cache;
          item_to_scm_marshaller = item_arg_cache->to_scm_marshaller;

          item_size = g_array_get_element_size (array_);

          for (i = 0; i < array_->len; i++)
            {
              GIArgument item_arg = {0};
              gpointer item_cleanup_data = NULL;
              SCM scm_item;

              // if we are receiving an array of pointers, simply assign the pointer
              // and move on, letting the per-item marshaller deal with the
              // various transfer odes and ref counts (e.g. g_variant_ref_sink).
              if (array_cache->array_type == GI_ARRAY_TYPE_PTR_ARRAY)
                {
                  item_arg.v_pointer = g_ptr_array_index ((GPtrArray *) array_, i);
                }
              else if (item_arg_cache->is_pointer)
                {
                  item_arg.v_pointer = g_array_index (array_, gpointer, i);
                }
              else if (item_arg_cache->type_tag == GI_TYPE_TAG_INTERFACE)
                {
                  GGIInterfaceCache *iface_cache = (GGIInterfaceCache *) item_arg_cache;

                  // FIXME: this probably doesn't work with bxoed type or gvalues
                  // see fx. _ggi_marshal_from_scm_array()

                  switch (g_base_info_get_type (iface_cache->interface_info))
                    {
                    case GI_INFO_TYPE_STRUCT:
                      if (arg_cache->transfer == GI_TRANSFER_EVERYTHING &&
                          !g_type_is_a (iface_cache->g_type, G_TYPE_BOXED))
                        {
                          // array elements are struct
                          gpointer *_struct = g_malloc (item_size);
                          memcpy (_struct, array_->data + i * item_size, item_size);
                          item_arg.v_pointer = _struct;
                        }
                      else
                        {
                          item_arg.v_pointer = array_->data + i * item_size;
                        }

                      break;
                    case GI_INFO_TYPE_ENUM:
                      memcpy (&item_arg, array_->data + i * item_size, item_size);
                      break;
                    default:
                      item_arg.v_pointer = g_array_index (array_, gpointer, i);
                      break;
                    }
                }
              else
                {
                  memcpy (&item_arg, array_->data + i * item_size, item_size);
                }

              scm_item = item_to_scm_marshaller (state,
                                                 callable_cache,
                                                 item_arg_cache,
                                                 &item_arg,
                                                 &item_cleanup_data);

              g_ptr_array_index (item_cleanups, i) = item_cleanup_data;

              if (scm_item == NULL)
                {
                  if (array_cache->array_type == GI_ARRAY_TYPE_C)
                    {
                      g_array_unref (array_);
                    }

                  g_ptr_array_unref (item_cleanups);

                  // goto error
                }

              scm_return_list = scm_append (scm_list_2 (scm_return_list, scm_list_1 (scm_item)));
              processed_items++;
            }
        }
    }

  if (array_cache->array_type == GI_ARRAY_TYPE_C)
    {
      g_array_free (array_, FALSE);
    }

  return scm_return_list;

  // TODO: err:
}

GGIArgCache *
ggi_arg_garray_len_arg_setup (GGIArgCache *arg_cache,
                              GITypeInfo *type_info,
                              GGICallableCache *callable_cache,
                              GGIDirection direction,
                              gssize arg_index,
                              gssize *scm_arg_index)
{
  GGIArgGArray *seq_cache = (GGIArgGArray *) arg_cache;

  // attempt len_arg_index setup for the first time 
  if (seq_cache->len_arg_index < 0)
    {
      seq_cache->len_arg_index = g_type_info_get_array_length (type_info);

      // offset by self arg for methods and vfuncs 
      if (seq_cache->len_arg_index >= 0 && callable_cache != NULL)
        {
          seq_cache->len_arg_index += callable_cache->args_offset;
        }
    }

  if (seq_cache->len_arg_index >= 0)
    {
      GGIArgCache *child_cache = NULL;

      child_cache = _ggi_callable_cache_get_arg (callable_cache, (guint)seq_cache->len_arg_index);
      if (child_cache == NULL)
        {
          child_cache = ggi_arg_cache_alloc ();
        }
      else
        {
          // If the "length" arg cache already exists (the length comes before
          // the array in the argument list), remove it from the to_py_args list
          // because it does not belong in "to python" return tuple. The length
          // will implicitly be a part of the returned Python list.
          if (direction & GGI_DIRECTION_TO_SCM)
            {
              callable_cache->to_scm_args =
                g_slist_remove (callable_cache->to_scm_args, child_cache);
            }

          // This is a case where the arg cache already exists and has been
          // setup by another array argument sharing the same length argument.
          // See: gi_marshalling_tests_multi_array_key_value_in
          if (child_cache->meta_type == GGI_META_ARG_TYPE_CHILD)
            {
              return child_cache;
            }
        }

      // There is a length argument for this array, so increment the number
      // of "to scm" child arguments when applicable.
      if (direction & GGI_DIRECTION_TO_SCM)
        {
          callable_cache->n_to_scm_child_args++;
        }

      child_cache->meta_type = GGI_META_ARG_TYPE_CHILD;
      child_cache->direction = direction;
      child_cache->to_scm_marshaller = ggi_marshal_to_scm_basic_type_cache_adapter;
      //        child_cache->from_py_marshaller = pygi_marshal_from_py_basic_type_cache_adapter;
      child_cache->scm_arg_index = -1;

      // ugly edge case code:
      // When the length comes before the array parameter we need to update
      // indexes of arguments after the index argument.
      if (seq_cache->len_arg_index < arg_index && direction & GGI_DIRECTION_FROM_SCM)
        {
          guint i;
          (*scm_arg_index) -= 1;
          callable_cache->n_scm_args -= 1;

          for (i = (guint) seq_cache->len_arg_index + 1;
               (gsize) i < _ggi_callable_cache_args_len (callable_cache); i++)
            {
              GGIArgCache *update_cache = _ggi_callable_cache_get_arg (callable_cache, i);
              if (update_cache == NULL)
                {
                  break;
                }

              update_cache->scm_arg_index -= 1;
            }
        }

      _ggi_callable_cache_set_arg (callable_cache, (guint) seq_cache->len_arg_index, child_cache);
      return child_cache;
    }

  return NULL;
}


static gboolean
_ggi_arg_garray_setup (GGIArgGArray            *sc,
                       GITypeInfo              *type_info,
                       GIArgInfo               *arg_info,
                       GITransfer               transfer,
                       GGIDirection             direction,
                       GGICallableCache        *callable_cache)
{
  GITypeInfo *item_type_info;
  GGIArgCache *arg_cache = (GGIArgCache *) sc;

  if (!ggi_arg_sequence_setup ((GGISequenceCache *) sc,
                               type_info,
                               arg_info,
                               transfer,
                               direction,
                               callable_cache))
    {
      return FALSE;
    }

  //  ((GGIArgCache *) sc)->destroy_notify = (GDestroyNotify) _array_cache_free_func;
  sc->array_type = g_type_info_get_array_type (type_info);
  sc->is_zero_terminated = g_type_info_is_zero_terminated (type_info);
  sc->fixed_size = g_type_info_get_array_fixed_size (type_info);
  sc->len_arg_index = -1; // setup by ggi_arg_garray_len_arg_setup

  item_type_info = g_type_info_get_param_type (type_info, 0);
  sc->item_size = _scmgi_c_g_type_info_size (item_type_info);
  g_base_info_unref ((GIBaseInfo *) item_type_info);

  if (direction & GGI_DIRECTION_FROM_SCM)
    {
      // TODO:
      //arg_cache->from_scm_marshaller = _ggi_marshal_from_scm_array;
      //      arg_cache->from_scm_cleanup = _ggi_marshal_cleanup_from_scm_array;
    }

  if (direction & GGI_DIRECTION_TO_SCM)
    {
      arg_cache->to_scm_marshaller = _ggi_marshal_to_scm_array;
      //      arg_cache->to_scm_cleanup = _ggi_marshal_cleanup_from_scm_array;
    }

  return TRUE;
}

GGIArgCache *
ggi_arg_garray_new_from_info (GITypeInfo       *type_info,
                              GIArgInfo        *arg_info,
                              GITransfer        transfer,
                              GGIDirection      direction,
                              GGICallableCache *callable_cache)
{
  GGIArgGArray *array_cache = g_slice_new0 (GGIArgGArray);

  if (array_cache == NULL)
    {
      return NULL;
    }

  if (!_ggi_arg_garray_setup (array_cache,
                              type_info,
                              arg_info,
                              transfer,
                              direction,
                              callable_cache))
    {
      // TODO: free
      return NULL;
    }

  return (GGIArgCache *) array_cache;
}
