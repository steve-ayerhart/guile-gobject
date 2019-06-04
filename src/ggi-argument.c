#include "ggi-argument.h"

SCM
ggi_arg_to_scm (GITypeInfo *arg_type,
                GITransfer transfer_type,
                GIArgument arg)
{
  switch (g_type_info_get_tag (arg_type))
    {
    case GI_TYPE_TAG_VOID:
      return PTR2SCM (arg.v_pointer);
    case GI_TYPE_TAG_BOOLEAN:
      return scm_from_bool (arg.v_boolean);
    case GI_TYPE_TAG_INT8:
      return scm_from_int8 (arg.v_int8);
    case GI_TYPE_TAG_UINT8:
      return scm_from_uint8 (arg.v_uint8);
    case GI_TYPE_TAG_INT16:
      return scm_from_int16 (arg.v_int16);
    case GI_TYPE_TAG_UINT16:
      return scm_from_uint16 (arg.v_uint16);
    case GI_TYPE_TAG_INT32:
      return scm_from_int32 (arg.v_int32);
    case GI_TYPE_TAG_UINT32:
      return scm_from_uint32 (arg.v_uint32);
    case GI_TYPE_TAG_INT64:
      return scm_from_int64 (arg.v_int64);
    case GI_TYPE_TAG_UINT64:
      return scm_from_uint64 (arg.v_uint64);
    case GI_TYPE_TAG_GTYPE:
      return scm_from_size_t (arg.v_size);
    case GI_TYPE_TAG_FLOAT:
      return scm_from_double (arg.v_float);
    case GI_TYPE_TAG_DOUBLE:
      return scm_from_double (arg.v_double);
    case GI_TYPE_TAG_FILENAME:
    case GI_TYPE_TAG_UTF8:
      return scm_from_locale_string (arg.v_string);
    case GI_TYPE_TAG_INTERFACE:
      return scm_from_bool (arg.v_boolean);

    default:
      return SCM_UNDEFINED;
    }
}
