
#include "gtype.h"

SCM_GLOBAL_SYMBOL (scm_sym_g_type, "g-type");

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
  #include "gtype.x"
  #endif

  scmg_type_metaclass = scm_make_foreign_object_type (scm_from_utf8_symbol ("<g-type-class>"),
                                                      scm_list_1 (scm_from_utf8_symbol ("g-type")),
                                                      NULL);
  scm_c_define ("<g-type-class>", scmg_type_metaclass);

  // (oop goops)
  scm_make_class = scm_permanent_object (scm_variable_ref (scm_c_lookup ("make-class")));
}
