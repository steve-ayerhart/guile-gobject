#include "ggutils.h"
#include "ggtype.h"

#include <stdio.h>
#include <string.h>

SCM_GLOBAL_SYMBOL (scm_sym_gtype, "gtype");
SCM_GLOBAL_SYMBOL (scm_sym_gtype_instance, "gtype-instance");

SCM scm_class_gtype_class;
SCM scm_class_gtype_instance;
SCM scm_sys_gtype_to_class;

SCM scm_class_gvalue;

SCM_SYMBOL (sym_name, "name");

SCM_KEYWORD (kw_name, "name");
SCM_KEYWORD (kw_class, "class");
SCM_KEYWORD (kw_metaclass, "metaclass");
SCM_KEYWORD (kw_gtype_name, "gtype-name");

static SCM scm_make_class;
static SCM scm_class_redefinition;
static SCM scm_allocate_instance;
static SCM scm_initialize;

static SCM scm_gtype_name_to_scheme_name;
static SCM scm_gtype_name_to_class_name;

static GQuark quark_class = 0;
static GQuark quark_type = 0;
static GQuark quark_guile_gtype_class = 0;
static GQuark guile_gobject_quark_wrapper;

static void scm_gtype_instance_unbind (scm_t_bits *slots);

typedef struct {
  GType type;
  void (* sinkfunc)(gpointer instance);
} SinkFunc;

static GSList *gtype_instance_funcs = NULL;
static GArray *sink_funcs = NULL;

SCM
scm_c_gtype_lookup_class (GType gtype)
{
  g_debug ("scm_c_gtype_lookup_class");

  SCM class;

  class = g_type_get_qdata (gtype, quark_class);

  return class ? class : SCM_BOOL_F;
}

static SCM
scm_c_gtype_get_direct_supers (GType type)
{
  g_debug ("scm_c_gtype_get_direct_supers");

  GType parent = g_type_parent (type);
  SCM supers = SCM_EOL;

  if (!parent)
    {
      if (G_TYPE_IS_INSTANTIATABLE (type))
        {
          g_debug (" gtype instance");

          supers = scm_cons (scm_class_gtype_instance, supers);
        }
      else
        {
          g_debug (" gvalue");

          supers = scm_cons (scm_class_gvalue, supers);
        }

    }
  else
    {
      g_debug (" no parent");

      SCM direct_super, cpl;
      GType *interfaces;
      guint n_interfaces, i;

      direct_super = scm_c_gtype_to_class (parent);
      cpl = scm_class_precedence_list (direct_super);
      supers = scm_cons (direct_super, supers);

      interfaces = g_type_interfaces (type, &n_interfaces);
      if (interfaces)
        {
          for (i = 0; i < n_interfaces; i++)
            {
              SCM i_class = scm_c_gtype_to_class (interfaces[i]);
              if (scm_is_false (scm_c_memq (i_class, cpl)))
                {
                  supers = scm_cons (i_class, supers);
                }
            }

          g_free (interfaces);
        }
    }

  return supers;
}

SCM
scm_c_gtype_to_class (GType gtype)
{
  g_debug ("scm_c_gtype_to_class: %s", g_type_name(gtype));

  SCM class, supers, gtype_name, name;

  class = scm_c_gtype_lookup_class (gtype);
  if (scm_is_true (class))
    {
      return class;
    }

  supers = scm_c_gtype_get_direct_supers (gtype);
  gtype_name = scm_from_locale_string (g_type_name (gtype));
  name = scm_call_1 (scm_gtype_name_to_class_name, gtype_name);

  class = scm_apply_0 (scm_make_class,
                       scm_list_n (supers, SCM_EOL,
                                   kw_gtype_name, gtype_name,
                                   kw_name, name, SCM_UNDEFINED));

  g_debug ("scm_c_gtype_to_class_done");
  return class;
}

SCM_DEFINE (scm_gtype_to_class, "gtype->class", 1, 0, 0,
            (SCM gtype),
            "")
{
  return scm_c_gtype_to_class (scm_to_ulong (gtype));
}

SCM_DEFINE (scm_gtype_name_to_class, "gtype-name->class", 1, 0, 0,
            (SCM name),
            "Return the @code{<gtype-class>} associated with the GType, @var{name}.")
#define FUNC_NAME s_scm_gtype_name_to_class
{
  GType type;
  gchar *c_name;

  SCM_VALIDATE_STRING (1, name);

  scm_dynwind_begin (0);
  c_name = scm_to_locale_string (name);
  scm_dynwind_free (c_name);

  type = g_type_from_name (c_name);
  if (!type)
    scm_misc_error (FUNC_NAME,
                    "No GType registered with name ~A",
                    scm_list_1 (name));

  scm_dynwind_end ();

  return scm_c_gtype_to_class (type);
}
#undef FUNC_NAME

static void
scm_gtype_instance_struct_free (SCM object)
{
  scm_gtype_instance_unbind (SCM_STRUCT_DATA (object));
}

GType
scm_c_gtype_class_to_gtype (SCM klass)
#define FUNC_NAME "gtype-class->gtype"
{
  SCM_VALIDATE_GTYPE_CLASS (1, klass);

  return scm_to_ulong (scm_slot_ref (klass, scm_sym_gtype));
}
#undef FUNC_NAME

gboolean
scm_c_gtype_class_is_a_p (SCM instance, GType gtype)
{
  return g_type_is_a (scm_c_gtype_class_to_gtype (instance), gtype);
}

// GTypeInstance

static scm_t_gtype_instance_funcs*
get_gtype_instance_instance_funcs (GType type)
{
  GSList *l;
  GType fundamental;

  fundamental = G_TYPE_FUNDAMENTAL (type);
  for (l = gtype_instance_funcs; l; l = l->next)
    {
      scm_t_gtype_instance_funcs *ret = l->data;
      if (fundamental == ret->type)
        {
          return ret;
        }
    }

  return NULL;
}

void
scm_register_gtype_instance_funcs (const scm_t_gtype_instance_funcs *funcs)
{
  gtype_instance_funcs = g_slist_append (gtype_instance_funcs, (gpointer) funcs);
}

gpointer
scm_c_gtype_instance_ref (gpointer instance)
{
  scm_t_gtype_instance_funcs *funcs;

  funcs = get_gtype_instance_instance_funcs (G_TYPE_FROM_INSTANCE (instance));
  if (funcs && funcs->ref)
    {
      funcs->ref (instance);
    }

  return instance;
}

void
scm_c_gtype_instance_unref (gpointer instance)
{
  scm_t_gtype_instance_funcs *funcs;

  funcs = get_gtype_instance_instance_funcs (G_TYPE_FROM_INSTANCE (instance));
  if (funcs && funcs->unref)
    {
      funcs->unref (instance);
    }
}

static SCM
scm_c_gtype_instance_get_cached (gpointer instance)
{
  SCM cached;
  scm_t_gtype_instance_funcs *funcs;

  funcs = get_gtype_instance_instance_funcs (G_TYPE_FROM_INSTANCE (instance));
  if (funcs && funcs->get_qdata)
    {
      gpointer data = funcs->get_qdata ((GObject *) instance, guile_gobject_quark_wrapper);

      if (data)
        {
          cached = GPOINTER_TO_SCM (data);
          return cached;
        }
    }

  return SCM_BOOL_F;
}

static void
scm_c_gtype_instance_set_cached (gpointer instance, SCM scm)
{
  scm_t_gtype_instance_funcs *funcs;

  funcs = get_gtype_instance_instance_funcs (G_TYPE_FROM_INSTANCE (instance));
  if (funcs && funcs->construct)
    {
      funcs->set_qdata ((GObject *) instance,
                        guile_gobject_quark_wrapper,
                        scm == SCM_BOOL_F ? NULL : SCM_TO_GPOINTER (scm));
    }
}

static gpointer
scm_c_gtype_instance_construct (SCM object, SCM initargs)
{
  GType type;
  scm_t_gtype_instance_funcs *funcs;
  type = scm_c_gtype_class_to_gtype (scm_class_of (object));
  funcs = get_gtype_instance_instance_funcs (type);
  if (funcs && funcs->construct)
    {
      return funcs->construct (object, initargs);
    }
  else
    {
      scm_misc_error ("gtype-instance-construct",
                      "Don't know how to construct instances of class ~A",
                      scm_list_1 (scm_c_gtype_to_class (type)));
    }

  return NULL;
}

static void
scm_c_gtype_instance_initialize_scm (SCM object, gpointer instance)
{
  GType type;
  scm_t_gtype_instance_funcs *funcs;

  type = scm_c_gtype_class_to_gtype (scm_class_of (object));
  funcs = get_gtype_instance_instance_funcs (type);
  if (funcs && funcs->initialize_scm)
    {
      funcs->initialize_scm (object, instance);
    }

}

static inline void
sink_type_instance (gpointer instance)
{
  if (sink_funcs)
    {
      gint i;

      for (i = 0; i < sink_funcs->len; i++)
        {
          if (g_type_is_a (G_TYPE_FROM_INSTANCE (instance),
                           g_array_index (sink_funcs, SinkFunc, i).type))
            {
              g_array_index (sink_funcs, SinkFunc, i).sinkfunc (instance);
            }

          break;
        }
    }
}

/**
 * As Guile handles memory management for us, the "floating reference" code in
 * GTK actually causes memory leaks for objects that are never parented. For
 * this reason, guile-gobject removes the floating references on objects on
 * construction.
 *
 * The sinkfunc should be able to remove the floating reference on
 * instances of the given type, or any subclasses.
 */
void
scm_register_gtype_instance_sinkfunc (GType type, void (*sinkfunc) (gpointer))
{
  SinkFunc sf;

  if (!sink_funcs)
    {
      
      sink_funcs = g_array_new (FALSE, FALSE, sizeof(SinkFunc));
    }

  sf.type = type;
  sf.sinkfunc = sinkfunc;
  g_array_append_val (sink_funcs, sf);
}

static void
scm_gtype_instance_unbind (scm_t_bits *slots)
{
  gpointer instance = (gpointer)slots[0];

  if (instance && instance != SCM_UNBOUND)
    {
      g_debug ("unbind c object 0x%p", instance);
      slots[0] = 0;
      scm_c_gtype_instance_set_cached (instance, SCM_BOOL_F);
      scm_c_gtype_instance_unref (instance);
    }
}

void
scm_c_gtype_instance_bind_to_object (gpointer ginstance, SCM object)
{
  scm_t_bits *slots = SCM_STRUCT_DATA (object);
  scm_c_gtype_instance_ref (ginstance);

  // sink the floating ref, if any
  sink_type_instance (ginstance);
  slots[0] = (scm_t_bits) ginstance;

  /* Cache the return value, so that if a callback or another function returns
   * this ginstance while the ginstance is visible elsewhere, the same wrapper
   * will be used. Released in unbind(). */
  scm_c_gtype_instance_set_cached (ginstance, object);

  g_debug ("bound SCM 0x%p to 0x%p", (void*)object, ginstance);
}

SCM_DEFINE (scm_sys_gtype_instance_construct, "%gtype-instance-initialize", 2, 0, 0,
            (SCM instance, SCM initargs),
            "")
{
  gpointer ginstance = (gpointer) SCM_STRUCT_DATA (instance)[0];

  scm_eval (scm_from_locale_string ("(next-method)"), scm_interaction_environment());

  if (ginstance && ginstance != (gpointer) SCM_UNBOUND) {
    scm_c_gtype_instance_initialize_scm (instance, ginstance);
  } else {
    gpointer new_ginstance;
    new_ginstance = scm_c_gtype_instance_construct (instance, initargs);
    ginstance = (gpointer) SCM_STRUCT_DATA (instance)[0];
    /* it's possible the construct function bound the object already, as is
     * the case for scheme-defined gobjects */

    if (new_ginstance != ginstance)
      scm_c_gtype_instance_bind_to_object (new_ginstance, instance);
    scm_c_gtype_instance_unref (new_ginstance);
  }

  return SCM_UNSPECIFIED;
}

SCM_DEFINE (scm_gtype_instance_destroy_x, "gtype-instance-destroy!", 1, 0, 0,
            (SCM instance),
            "Release all  "
            "However some @code{<gtype-class>} instances have semantics that "
            "require this function. The canonical example is that when a "
            "@code{<gtk-object>} emits the @code{destroy} signal, all "
            "code should drop their references to the object. This is, "
            "of course, handled internally in the @code{(gnome gtk)} "
            "module.")
#define FUNC_NAME s_scm_gtype_instance_destroy_x
{
  SCM_VALIDATE_GTYPE_INSTANCE (1, instance);
  scm_gtype_instance_unbind (SCM_STRUCT_DATA (instance));

  return SCM_UNSPECIFIED;
}
#undef FUNC_NAME

gpointer
scm_c_scm_to_gtype_instance (SCM scm_instance)
{
  SCM scm_ulong;
  gpointer c_ginstance;

  if (!SCM_IS_A_P (scm_instance, scm_class_gtype_instance))
    {
      return NULL;
    }

  scm_ulong = scm_from_ulong (SCM_STRUCT_DATA (scm_instance)[0]);

  if (scm_ulong == SCM_UNBOUND)
    {
      scm_misc_error ("scm->gtype-instance",
                      "Object ~A is uninitialized.",
                      scm_list_1 (scm_instance));
      
    }

  c_ginstance = (gpointer) scm_to_ulong (scm_ulong);

  if (!c_ginstance)
    {
      scm_misc_error ("scm->gtype-instance",
                      "Object ~A has been destroyed.",
                      scm_list_1 (scm_instance));
      
    }

  return c_ginstance;
}

gboolean
scm_c_gtype_instance_is_a_p (SCM scm_instance, GType c_gtype)
{
  return scm_c_scm_to_gtype_instance_typed (scm_instance, c_gtype) != NULL;
}

gpointer
scm_c_scm_to_gtype_instance_typed (SCM scm_instance, GType c_gtype)
{
  gpointer c_ginstance = scm_c_scm_to_gtype_instance (scm_instance);

  if (!G_TYPE_CHECK_INSTANCE_TYPE (c_ginstance, c_gtype))
    {
      return NULL;
    }

  return c_ginstance;
}

/* returns a goops object of class (gtype->class type). this function exists for
 * gobject.c:scm_c_gtype_instance_instance_init. all other callers should use
 * scm_c_gtype_instance_to_scm. */
SCM
scm_c_gtype_instance_to_scm_typed (gpointer c_ginstance, GType c_gtype)
{
  g_debug ("scm_c_gtype_instance_to_scm_typed");

  SCM scm_class, scm_object;

  scm_object = scm_c_gtype_instance_get_cached (c_ginstance);
  if (scm_is_true (scm_object))
    {
      return scm_object;
    }

  scm_class = scm_c_gtype_lookup_class (c_gtype);

  if (scm_is_false (scm_class))
    {
      scm_class = scm_c_gtype_to_class (c_gtype);
    }

  g_assert (scm_is_true (scm_class));

  // FIXME more comments on why we do it this way
  scm_object = scm_call_2 (scm_allocate_instance, scm_class, SCM_EOL);

  scm_c_gtype_instance_bind_to_object (c_ginstance, scm_object);
  scm_call_2 (scm_initialize, scm_object, SCM_EOL);

  return scm_object;
}

SCM
scm_c_gtype_instance_to_scm (gpointer c_ginstance)
{
  if (!c_ginstance)
    {
      return SCM_BOOL_F;
    }

  return scm_c_gtype_instance_to_scm_typed (c_ginstance, G_TYPE_FROM_INSTANCE (c_ginstance));
}

SCM_DEFINE (scm_gtype_class_bind, "%gtype-class-bind", 2, 0, 0,
            (SCM scm_gtype_class, SCM scm_gtype_name),
            "")
#define FUNC_NAME "initialize"
{
  g_debug ("_scm_gtype_class_bind");

  GType gtype;
  char *gtype_name;

  SCM_VALIDATE_GTYPE_CLASS (1, scm_gtype_class);

  if (scm_c_gtype_class_to_gtype (scm_gtype_class))
    {
      scm_misc_error ("initialize", "class '~a' already has a GType", scm_list_1 (scm_gtype_name));
    }

  if (scm_is_symbol (scm_gtype_name))
    {
      scm_gtype_name = scm_symbol_to_string (scm_gtype_name);
    }
  //else
  //  scm_misc_error ("initialize", "#:gtype-name must be a symbol or #t", SCM_EOL);

  scm_dynwind_begin (0);
  gtype_name = scm_to_locale_string (scm_gtype_name);
  scm_dynwind_free (gtype_name);

  gtype = g_type_from_name (gtype_name);
  if (!gtype)
    {
      scm_misc_error ("initialize", "No GType registered with name '~a'", scm_list_1 (scm_gtype_name));
    }

  if (scm_is_true (scm_c_gtype_lookup_class (gtype)))
    {
      scm_misc_error ("initialize", "'~a' already has a GOOPS class, use gtype-name->class", scm_list_1 (scm_gtype_name));
    }

  g_type_set_qdata (gtype, quark_class, scm_permanent_object (scm_gtype_class));
  scm_slot_set_x (scm_gtype_class, scm_sym_gtype, scm_from_ulong (gtype));

  scm_dynwind_end ();

  return SCM_UNSPECIFIED;
}

SCM_DEFINE (scm_gtype_inherit_magic, "%gtype-class-inherit-magic", 1, 0, 0,
            (SCM scm_gtype_class),
            "")
#define FUNC_NAME "initialize"
{
  g_debug ("_scm_gtype_inherit_magic");
  GType gtype;
  scm_t_bits *slots;

  SCM_VALIDATE_GTYPE_CLASS_COPY (1, scm_gtype_class, gtype);

  slots = SCM_STRUCT_DATA (scm_gtype_class);
  // inherit class free function
  if (g_type_parent (gtype))
    {
      SCM parent = scm_c_gtype_to_class (g_type_parent (gtype));
      slots[scm_vtable_index_instance_finalize] =
        SCM_STRUCT_DATA (parent)[scm_vtable_index_instance_finalize];
    }
  else if (G_TYPE_IS_INSTANTIATABLE (gtype))
    {
      slots[scm_vtable_index_instance_finalize] =
        (scm_t_bits) scm_gtype_instance_struct_free;
    }
  else
    {
      SCM parent = scm_cadr (scm_class_precedence_list (scm_gtype_class));
      // is this right? layout might not be the same
      slots[scm_vtable_index_instance_finalize] =
        SCM_STRUCT_DATA (parent)[scm_vtable_index_instance_finalize];
    }

  g_debug ("_scm_gtype_inherit_magic_end");
  return SCM_UNSPECIFIED;
}
#undef FUNC_NAME

void
ggobject_gtype_init (void)
{
  g_debug ("ggobject_gtype_init");

#ifndef SCM_MAGIC_SNARFER
#include "ggtype.x"
#endif

  quark_type = g_quark_from_static_string ("scm-gtype->type");
  quark_class = g_quark_from_static_string ("scm-gtype->class");
  quark_guile_gtype_class = g_quark_from_static_string ("scm-guile-gtype-class");
  guile_gobject_quark_wrapper = g_quark_from_static_string ("guile-gobject-wrapper");

  scm_sys_gtype_to_class =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("gtype->class")));

  scm_class_gtype_class =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("<gtype-class>")));
  scm_class_gtype_instance =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("<gtype-instance>")));

  // (gobject utils)
  scm_gtype_name_to_scheme_name =
    scm_permanent_object (scm_variable_ref (scm_c_public_lookup ("gobject utils", "gtype-name->scheme-name")));
  scm_gtype_name_to_class_name =
    scm_permanent_object (scm_variable_ref (scm_c_public_lookup ("gobject utils", "gtype-name->class-name")));

  // (oop goops)
  scm_make_class = scm_permanent_object (scm_variable_ref (scm_c_lookup ("make-class")));
  scm_class_redefinition =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("class-redefinition")));
  scm_allocate_instance =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("allocate-instance")));
  scm_initialize =
    scm_permanent_object (scm_variable_ref (scm_c_lookup ("initialize")));

  g_debug ("ggobject_gtype_end");
}
