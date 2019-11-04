(define-module (g-object g-parameter)
  #:use-module (g-object config)
  #:use-module (g-object utils)

  #:use-module (g-object g-type)
  #:use-module (g-object g-value)

  #:use-module (oop goops)

  #:export (<g-param>
            <g-param-char> <g-param-uchar> <g-param-boolean> <g-param-int>
            <g-param-uint> <g-param-long> <g-param-ulong> <g-param-int64>
            <g-param-uint64> <g-param-float> <g-param-double>
            <g-param-unichar> <g-param-pointer> <g-param-string>
            <g-param-boxed> <g-param-enum> <g-param-flags> <g-param-g-type>
            ;; Helper class
            <g-param-spec-flags>
            ;; Limits
            gparameter:uint-max gparameter:int-min gparameter:int-max
            gparameter:ulong-max gparameter:long-min
            gparameter:long-max gparameter:uint64-max
            gparameter:int64-min gparameter:int64-max
            gparameter:float-max gparameter:float-min
            gparameter:double-max gparameter:double-min
            gparameter:byte-order))

(define-class/docs <g-param-spec-flags> (<g-flags>)
  "A @code{<gflags>} type for the flags allowable on a @code{<g-param>}:
@code{read}, @code{write}, @code{construct}, @code{construct-only}, and
@code{lax-validation}."
  #:vtable
  #((read "Readable" 1)
    (write "Writable" 2)
    (construct "Set on object construction" 4)
    (construct-only "Only set on object construction" 8)
    (lax-validation "Don't require strict validation on parameter conversion" 16)))

;; The C code needs to reference <g-param> for use in its predicates.
;; Define it now before loading the library.
(define-class <g-param-class> (<g-type-class>)
  (value-type #:init-keyword #:value-type))

(define-method (compute-get-n-set (class <g-param-class>) s)
  (case (slot-definition-allocation s)
    ((#:checked)
     (let ((already-allocated (slot-ref class 'nfields))
           (pred (get-keyword #:pred (slot-definition-options s) #f))
           (trans (get-keyword #:trans (slot-definition-options s) #f)))
       (or pred (error "Missing #:pred for #:checked slot"))
       ;; allocate a field in the struct
       (slot-set! class 'nfields (+ already-allocated 1))
       ;; struct-ref and struct-set! don't work on the structs that back
       ;; GOOPS objects, because they are "light structs", without the
       ;; hidden word that says how many fields are in the struct.
       ;; Patches submitted to guile-devel on 10 April 2008. Until then,
       ;; use our own struct accessors.
       (list (lambda (instance)
               (hacky-struct-ref instance already-allocated))
             (lambda (instance value)
               (let ((value (if trans (trans value) value)))
                 (if (pred value)
                     (hacky-struct-set! instance already-allocated value)
                     (error
                      "Bad value for slot ~A on instance ~A: ~A"
                      (slot-definition-name s) instance value)))))))

    (else (next-method))))

(define-class/docs <g-param> (<g-type-instance>)
  "The base class for GLib parameter objects. (Doc slots)"
  (name #:init-keyword #:name #:allocation #:checked #:pred symbol?)
  (nick #:init-keyword #:nick #:allocation #:checked #:pred string?)
  (blurb #:init-keyword #:blurb #:allocation #:checked #:pred string?)
  (flags #:init-keyword #:flags #:init-value '(read write)
         #:allocation #:checked #:pred number?
         #:trans (lambda (x)
                   (apply + (gflags->value-list
                             (make <g-param-spec-flags> #:value x)))))
  #:g-type-name "GParam"
  #:metaclass <g-param-class>)


(eval-when (expand load eval)
  (load-extension (extensions-library "gobject") "scm_gobject_gparameter_init"))

(define-class/docs <g-param-char> (<g-param>)
  "Parameter for @code{<gchar>} values."
  (minimum
   #:init-keyword #:minimum #:init-value (integer->char 0)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  (maximum
   #:init-keyword #:maximum #:init-value (integer->char 127)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  (default-value
   #:init-keyword #:default-value #:init-value (integer->char 127)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  #:value-type <gchar>
  #:g-type-name "GParamChar")

(define-class/docs <g-param-uchar> (<g-param>)
  "Parameter for @code{<guchar>} values."
  (minimum
   #:init-keyword #:minimum #:init-value (integer->char 0)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  (maximum
   #:init-keyword #:maximum #:init-value (integer->char 255)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  (default-value
   #:init-keyword #:default-value #:init-value (integer->char 255)
   #:allocation #:checked #:pred (lambda (x) (or (char? x) (integer? x))))
  #:value-type <guchar>
  #:g-type-name "GParamUChar")

(define-class/docs <g-param-boolean> (<g-param>)
  "Parameter for @code{<gboolean>} values."
  (default-value
   #:init-keyword #:default-value #:init-value #f
   #:allocation #:checked #:pred boolean?)
  #:value-type <gboolean>
  #:g-type-name "GParamBoolean")

(define-class/docs <g-param-int> (<g-param>)
  "Parameter for @code{<gint>} values."
  (minimum
   #:init-keyword #:minimum #:init-value gparameter:int-min
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:int-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <gint>
  #:g-type-name "GParamInt")

(define-class/docs <g-param-uint> (<g-param>)
  "Parameter for @code{<guint>} values."
  (minimum
   #:init-keyword #:minimum #:init-value 0
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:uint-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <guint>
  #:g-type-name "GParamUInt")

(define-class/docs <g-param-unichar> (<g-param>)
  "Parameter for Unicode codepoints, represented as @code{<guint>}
values."
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <guint>
  #:g-type-name "GParamUnichar")

(define-class/docs <g-param-long> (<g-param>)
  "Parameter for @code{<glong>} values."
  (minimum
   #:init-keyword #:minimum #:init-value gparameter:long-min
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:long-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <glong>
  #:g-type-name "GParamLong")

(define-class/docs <g-param-ulong> (<g-param>)
  "Parameter for @code{<gulong>} values."
  (minimum
   #:init-keyword #:minimum #:init-value 0
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:ulong-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <gulong>
  #:g-type-name "GParamULong")

(define-class/docs <g-param-int64> (<g-param>)
  "Parameter for @code{<gint64>} values."
  (minimum
   #:init-keyword #:minimum #:init-value gparameter:int64-min
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:int64-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <gint64>
  #:g-type-name "GParamInt64")

(define-class/docs <g-param-uint64> (<g-param>)
  "Parameter for @code{<guint64>} values."
  (minimum
   #:init-keyword #:minimum #:init-value 0
   #:allocation #:checked #:pred integer?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:uint64-max
   #:allocation #:checked #:pred integer?)
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred integer?)
  #:value-type <guint64>
  #:g-type-name "GParamUInt64")

(define-class/docs <g-param-float> (<g-param>)
  "Parameter for @code{<gfloat>} values."
  (minimum
   #:init-keyword #:minimum #:init-value (- gparameter:float-max)
   #:allocation #:checked #:pred real?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:float-max
   #:allocation #:checked #:pred real?)
  (default-value
   #:init-keyword #:default-value #:init-value 0.0
   #:allocation #:checked #:pred real?)
  #:value-type <gfloat>
  #:g-type-name "GParamFloat")

(define-class/docs <g-param-double> (<g-param>)
  "Parameter for @code{<gdouble>} values."
  (minimum
   #:init-keyword #:minimum #:init-value (- gparameter:double-max)
   #:allocation #:checked #:pred real?)
  (maximum
   #:init-keyword #:maximum #:init-value gparameter:double-max
   #:allocation #:checked #:pred real?)
  (default-value
   #:init-keyword #:default-value #:init-value 0.0
   #:allocation #:checked #:pred real?)
  #:value-type <gdouble>
  #:g-type-name "GParamDouble")

(define-class/docs <g-param-pointer> (<g-param>)
  "Parameter for @code{<gpointer>} values."
  #:value-type <gpointer>
  #:g-type-name "GParamPointer")

(define-class/docs <g-param-string> (<g-param>)
  "Parameter for @code{<gchar-array>} values."
  (default-value
   #:init-keyword #:default-value #:init-value ""
   #:allocation #:checked #:pred (lambda (x) (or (not x) (string? x))))
  #:value-type <gchar-array>
  #:g-type-name "GParamString")

(define (class-is-a? x is-a)
  (memq is-a (class-precedence-list x)))

(define-class/docs <g-param-boxed> (<g-param>)
  "Parameter for @code{<g-boxed>} values."
  (boxed-type
   #:init-keyword #:boxed-type #:allocation #:checked
   #:pred (lambda (x) (class-is-a? x <g-boxed>)))
  #:value-type <g-boxed>
  #:g-type-name "GParamBoxed")

(define-class/docs <g-param-enum> (<g-param>)
  "Parameter for @code{<g-enum>} values."
  (enum-type
   #:init-keyword #:enum-type #:allocation #:checked
   #:pred (lambda (x) (class-is-a? x <g-enum>)))
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred number?)
  #:value-type <g-enum>
  #:g-type-name "GParamEnum")

(define-class/docs <g-param-flags> (<g-param>)
  "Parameter for @code{<g-flags>} values."
  (flags-type
   #:init-keyword #:flags-type #:allocation #:checked
   #:pred (lambda (x) (class-is-a? x <g-flags>)))
  (default-value
   #:init-keyword #:default-value #:init-value 0
   #:allocation #:checked #:pred number?)
  #:value-type <g-flags>
  #:g-type-name "GParamFlags")

(define-class/docs <g-param-value-array> (<g-param>)
  "Parameter for @code{<gvalue-array>} values."
  (element-spec
   #:init-keyword #:element-spec #:allocation #:checked
   #:pred (lambda (x) (or (not x) (is-a? x <g-param>))))
  #:value-type <g-value-array>
  #:g-type-name "GParamValueArray")

(define-class/docs <g-param-g-type> (<g-param>)
  "Parameter for @code{<g-type>} values."
  (is-a-type
   #:init-keyword #:is-a-type #:allocation #:checked
   #:pred (lambda (x) (or (not x) (is-a? x <g-type-class>))))
  #:value-type <g-type-class>
  #:g-type-name "GParamGType")

;;;
;;; {Instance Initialization}
;;;

;; fixme, make me more useful
(define-method (write (param <g-param>) file)
  (let ((class (class-of param))
        (loc (number->string (object-address param) 16)))
    (if (slot-bound? class 'name)
        (with-accessors (name)
          (format file "<~a ~a ~a>" (class-name class) (name param) loc))
        (format file "<~a (uninitialized) ~a>" (class-name class) loc))))
