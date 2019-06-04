
#include "ggtype.h"

SCM_GLOBAL_SYMBOL (scm_sym_g_type, "g-type");

SCM_GLOBAL_SYMBOL (scm_sym_g_type_invalid, "g-type-invalid");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_none, "g-type-none");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_interface, "g-type-interface");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_char, "g-type-char");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_uchar, "g-type-uchar");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_boolean, "g-type-boolean");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_int, "g-type-int");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_uint, "g-type-uint");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_long, "g-type-long");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_ulong, "g-type-ulong");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_int64, "g-type-int64");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_uint64, "g-type-uint64");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_enum, "g-type-enum");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_flags, "g-type-flags");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_float, "g-type-float");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_double, "g-type-double");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_str, "g-type-string");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_pointer, "g-type-pointer");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_boxed, "g-type-boxed");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_param, "g-type-param");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_object, "g-type-object");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_g_type, "g-type-g-type");
SCM_GLOBAL_SYMBOL (scm_sym_g_type_variant, "g-type-variant");

SCM_KEYWORD (scm_kw_g_type, "g-type");

SCM_DEFINE (scmg_type_allocate_instance, "%allocate-instance", 1, 0, 0,
            (SCM scm_g_type_class),
            "")
{
  return scm_make_foreign_object_1 (scmg_type_metaclass, (void *) G_TYPE_NONE);
}

SCM_DEFINE( scmg_type_initialize, "%initialize", 2, 0, 0,
            (SCM scm_g_type_instance, SCM scm_initargs),
            ""
            )
{
  SCM scm_g_type;

  scm_g_type = SCM_UNDEFINED;
  scm_c_bind_keyword_arguments ("initialize", scm_initargs, SCM_ALLOW_OTHER_KEYS,
                                scm_kw_g_type, &scm_g_type,
                                SCM_UNDEFINED);

  if (SCM_UNBNDP (scm_g_type))
    scm_misc_error ("initialize", "Need #:g-type initarg", SCM_EOL);

  scm_foreign_object_set_x (scm_g_type_instance, 0, (void *) scm_to_ulong (scm_g_type));

  return SCM_UNSPECIFIED;
}

void
scmg_type_init (void)
{
  g_debug ("scmg_type_init");

#ifndef SCM_MAGIC_SNARFER
#include "ggtype.x"
#endif

  // symbols
  SET_GTYPE_SYMBOL (scm_sym_g_type_invalid, G_TYPE_INVALID);
  SET_GTYPE_SYMBOL (scm_sym_g_type_none, G_TYPE_NONE);
  SET_GTYPE_SYMBOL (scm_sym_g_type_interface, G_TYPE_INTERFACE);
  SET_GTYPE_SYMBOL (scm_sym_g_type_char, G_TYPE_CHAR);
  SET_GTYPE_SYMBOL (scm_sym_g_type_uchar, G_TYPE_UCHAR);
  SET_GTYPE_SYMBOL (scm_sym_g_type_boolean, G_TYPE_BOOLEAN);
  SET_GTYPE_SYMBOL (scm_sym_g_type_int, G_TYPE_INT);
  SET_GTYPE_SYMBOL (scm_sym_g_type_uint, G_TYPE_UINT);
  SET_GTYPE_SYMBOL (scm_sym_g_type_long, G_TYPE_LONG);
  SET_GTYPE_SYMBOL (scm_sym_g_type_ulong, G_TYPE_ULONG);
  SET_GTYPE_SYMBOL (scm_sym_g_type_int64, G_TYPE_INT64);
  SET_GTYPE_SYMBOL (scm_sym_g_type_uint64, G_TYPE_UINT64);
  SET_GTYPE_SYMBOL (scm_sym_g_type_enum, G_TYPE_ENUM);
  SET_GTYPE_SYMBOL (scm_sym_g_type_flags, G_TYPE_FLAGS);
  SET_GTYPE_SYMBOL (scm_sym_g_type_float, G_TYPE_FLOAT);
  SET_GTYPE_SYMBOL (scm_sym_g_type_double, G_TYPE_DOUBLE);
  SET_GTYPE_SYMBOL (scm_sym_g_type_str, G_TYPE_STRING);
  SET_GTYPE_SYMBOL (scm_sym_g_type_pointer, G_TYPE_POINTER);
  SET_GTYPE_SYMBOL (scm_sym_g_type_boxed, G_TYPE_BOXED);
  SET_GTYPE_SYMBOL (scm_sym_g_type_param, G_TYPE_PARAM);
  SET_GTYPE_SYMBOL (scm_sym_g_type_object, G_TYPE_OBJECT);
  SET_GTYPE_SYMBOL (scm_sym_g_type_g_type, G_TYPE_GTYPE);
  SET_GTYPE_SYMBOL (scm_sym_g_type_variant, G_TYPE_VARIANT);

  scmg_type_metaclass = scm_make_foreign_object_type (scm_from_utf8_symbol ("<g-type-class>"),
                                                      scm_list_1 (scm_from_utf8_symbol ("g-type")),
                                                      NULL);
  scm_c_define ("<g-type-class>", scmg_type_metaclass);

  // (oop goops)
  scm_make_class = scm_permanent_object (scm_variable_ref (scm_c_lookup ("make-class")));
}
