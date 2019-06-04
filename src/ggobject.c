#include "ggutils.h"

#include "ggobject.h"

SCM_GLOBAL_SYMBOL (scm_sym_g_object, "g-object");

SCM_KEYWORD (scm_kw_g_type_name, "g-type-name");
SCM_KEYWORD (scm_kw_name, "name");
SCM_KEYWORD (scm_kw_metaclass, "metaclass");
SCM_KEYWORD (scm_kw_class, "class");

static SCM scm_make_class;
static SCM ggtype_name_to_scheme_name;
static SCM ggtype_name_to_class_name;

SCM scm_class_goops;
SCM scm_class_gtype_instance;

static GQuark quark_class = 0;

static SCM
ggobject_lookup_class (GType gtype)
{
  SCM scm_class;

  scm_class = g_type_get_qdata (gtype, quark_class);

  return scm_class ? scm_class : SCM_BOOL_F;
}

static SCM
ggtype_to_class (GType gtype)
{
  SCM scm_class, scm_supers, scm_g_type_name, scm_name;

  scm_class = ggobject_lookup_class (gtype);

  if (scm_is_true (scm_class))
    return scm_class;

  scm_supers = ggobject_get_direct_supers (gtype);
  scm_g_type_name = scm_from_locale_string (g_type_name (gtype));
  scm_name = scm_call_1 (ggtype_name_to_class_name, scm_g_type_name);

  scm_class = scm_apply_0 (scm_make_class,
                           scm_list_n (scm_supers, SCM_EOL,
                                       scm_kw_g_type_name, scm_g_type_name,
                                       scm_kw_name, scm_name,
                                       SCM_UNDEFINED));
  return scm_class;
}

static SCM
ggobject_get_direct_supers (GType gtype)
{
  GType parent = g_type_parent (gtype);
  SCM scm_supers = SCM_EOL;

  if (!parent)
    {
      if (G_TYPE_IS_INSTANTIATABLE (gtype))
        scm_supers = scm_cons (scm_class_gtype_instance, scm_supers);
      else
        scm_supers = scm_cons (scm_class_goops, scm_supers);
    }
  else
    {
      GType *interfaces;
      guint n_interfaces, i;
      SCM scm_direct_super, cpl;

      scm_direct_super = ggtype_to_class (parent);
      cpl = scm_class_precedence_list (scm_direct_super);
      scm_supers = scm_cons (scm_direct_super, scm_supers);

      interfaces = g_type_interfaces (gtype, &n_interfaces);
      if (interfaces)
        {
          for (i= 0; i < n_interfaces; i++)
            {
              SCM scm_interface_class = ggtype_to_class (interfaces[i]);
              if (scm_is_false (scm_c_memq (scm_interface_class, cpl)))
                scm_supers = scm_cons (scm_interface_class, scm_supers);
            }

          g_free (interfaces);
        }
    }

  return scm_supers;
}
/*
GType
gg_g_type_from_object (SCM scm_object)
{
  GType gtype;
  SCM scm_gtype;
  SCM scm_c_is_a_p;

  scm_c_is_a_p = scm_variable_ref (scm_c_public_lookup ("oop goops", "is-a?"));

  if (scm_is_true (scm_c_is_a_p (scm_object, s)))
}
*/

void
ggobject_init (void)
{
  g_debug ("ggobject_init");
  ggutils_init ();

  #ifndef SCM_MAGIC_SNARFER
  #include "ggobject.x"
  #endif

  ggtype_name_to_scheme_name =
    scm_permanent_object (scm_variable_ref (scm_c_public_lookup ("g-object utils", "g-type-name->scheme-name")));
  ggtype_name_to_class_name =
    scm_permanent_object (scm_variable_ref (scm_c_public_lookup ("g-object utils", "g-type-name->class-name")));

  // (oop goops)
  scm_make_class = scm_permanent_object (scm_variable_ref (scm_c_lookup ("make-class")));
}
