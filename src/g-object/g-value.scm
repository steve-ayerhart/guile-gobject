(define-module (g-object g-value)
  #:use-module (g-object config)
  #:use-module (g-object utils)
  #:use-module (g-object g-type)
  #:use-module (oop goops)


  #:export (<g-value>

            <gboolean> <gchar> <guchar> <gint> <guint> <glong>
            <gulong> <gint64> <guint64> <gfloat> <gdouble>
            <gchar-array> <g-boxed> <g-boxed-scm> <g-value-array>
            <gpointer>
            <g-enum> <g-flags>
            genum-register-static gflags-register-static
            genum-class->value-table gflags-class->value-table
            scm->gvalue gvalue->scm
            genum->symbol genum->name genum->value genum->enum
            gflags->value
            gflags->symbol-list gflags->name-list gflags->value-list))

;;; generic values

(eval-when (expand load eval)
  (define-class <g-value-class> (<g-type-class>))

  (define-class <g-value> ()
    (value #:class <read-only-slot>)
    #:g-type-name #t
    #:metaclass <g-value-class>)

  (load-extension (extensions-library "gobject") "scm_gobject_gvalue_init"))

(eval-when (load eval)
  (bless-gvalue-class <g-value>))

(define-method (allocate-instance (class <g-value-class>) initargs)
  (let ((instance (next-method)))
    (allocate-gvalue class instance)
    instance))

(define-method (initialize (instance <g-value>) initargs)
  (or (memq #:value initargs)
      (error "Missing #:value argument"))
  (gvalue-set! instance (get-keyword #:value initargs 'foo)))

(define-method (write (obj <g-value>) file)
  (format file "#<~a ~a ~a>"
          (class-name (class-of obj))
          (number->string (object-address obj) 16)
          (let ((converted (gvalue->scm obj)))
            ;; should be able to use eq?, but gvalue->scm always rips us
            ;; a fresh one. bugs, bugs..
            (if (is-a? converted <g-value>)
                "[native]"
                converted))))

;; simple classes

(define-class/docs <gchar> (<g-value>)
  "A @cod{<g-value>} class for signed 8-bit values"
  #:g-type-name "gchar")

(define-class/docs <guchar> (<g-value>)
  "A @code{<g-value>} class for unsigned 8-bit values."
  #:g-type-name "guchar")

(define-class/docs <gboolean> (<g-value>)
  "A @code{<g-value>} class for boolean values."
  #:g-type-name "gboolean")

(define-class/docs <gint> (<g-value>)
  "A @code{<g-value>} class for signed 32-bit values."
  #:g-type-name "gint")

(define-class/docs <guint> (<g-value>)
  "A @code{<g-value>} class for unsigned 32-bit values."
  #:g-type-name "guint")

(define-class/docs <glong> (<g-value>)
  "A @code{<g-value>} class for signed ``long'' (32- or 64-bit)
values."
  #:g-type-name "glong")

(define-class/docs <gulong> (<g-value>)
  "A @code{<g-value>} class for unsigned ``long'' (32- or 64-bit)
values."
  #:g-type-name "gulong")

(define-class/docs <gint64> (<g-value>)
  "A @code{<g-value>} class for signed 64-bit values."
  #:g-type-name "gint64")

(define-class/docs <guint64> (<g-value>)
  "A @code{<g-value>} class for unsigned 64-bit values."
  #:g-type-name "guint64")

(define-class/docs <gfloat> (<g-value>)
  "A @code{<g-value>} class for 32-bit floating-point values."
  #:g-type-name "gfloat")

(define-class/docs <gdouble> (<g-value>)
  "A @code{<g-value>} class for 64-bit floating-point values."
  #:g-type-name "gdouble")

(define-class/docs <gchar-array> (<g-value>)
  "A @code{<g-value>} class for arrays of 8-bit values (C strings)."
  #:g-type-name "gchararray")

(define-class/docs <g-boxed> (<g-value>)
  "A @code{<g-value>} class for ``boxed'' types, a way of wrapping
generic C structures. You won't see instances of this class, only of its
subclasses."
  #:g-type-name "GBoxed")


(define-class/docs <g-value-array> (<g-boxed>)
  "A @code{<g-value>} class for arrays of @code{<g-value>}."
  #:g-type-name "GValueArray")

(define-class/docs <g-boxed-scm> (<g-boxed>)
  "A @code{<g-boxed>} class for holding arbitrary Scheme objects."
  #:g-type-name "GBoxedSCM")

(define-class/docs <gpointer> (<g-value>)
  "A @code{<g-value>} class for opaque pointers."
  #:g-type-name "gpointer")

;;; enums

(define (vtable-ref vtable keyfunc val)
  (let lp ((i (1- (vector-length vtable))))
    (cond ((< i 0) (error "No such value in ~A: ~A" vtable val))
          ((equal? (keyfunc (vector-ref vtable i)) val)
           (vector-ref vtable i))
          (else (lp (1- i))))))

(define vtable-symbol car)
(define vtable-name cadr)
(define vtable-index caddr)
(define (vtable-by-value vtable v)
  (vtable-ref vtable
              (cond ((integer? v) vtable-index)
                    ((symbol? v) vtable-symbol)
                    ((string? v) vtable-name)
                    (else (error "Wrong type argument: ~S" v)))
              v))

(define-class <g-enum-class> (<g-value-class>))

(define-class/docs <g-enum> (<g-value>)
  "A @code{<g-value>} base class for enumerated values. Users may define
new enumerated value types via subclssing from @code{<g-enum>}, passing
@code{#:vtable @var{table}} as an initarg, where @var{table} should be
in a format suitable for passing to @code{genum-register-static}."
  #:g-type-name "GEnum"
  #:metaclass <g-enum-class>)

(define (has-kw-arg? args key)
  (cond ((null? args) #f)
        ((eq? (car args) key) #t)
        (else (has-kw-arg? (cddr args) key))))

(define (supply-initarg-if-missing initargs key proc)
  (cond ((has-kw-arg? initargs key) initargs)
        (else (cons* key (proc initargs) initargs))))

(define (override-initarg initargs key val)
  (cons* key val initargs))

(define (kw-ref initargs key)
  (or (has-kw-arg? initargs key)
      (error "Missing required keyword argument:" key))
  (get-keyword key initargs #f))

(define-method (initialize (class <g-enum-class>) initargs)
  (next-method class
               (supply-initarg-if-missing
                initargs #:g-type-name
                (lambda (initargs)
                  (let ((name (class-name->gtype-name (kw-ref initargs #:name))))
                    (genum-register-static name (kw-ref initargs #:vtable))
                    name)))))

(define-method (initialize (instance <g-enum>) initargs)
  (next-method
   instance
   (override-initarg
    initargs #:value
    (vtable-index
     (vtable-by-value
      (genum-class->value-table (class-of instance))
      (kw-ref initargs #:value))))))

(define-method (write (obj <g-enum>) file)
  (format file "#<~a ~a ~a>"
          (class-name (class-of obj))
          (number->string (object-address obj) 16)
          (genum->symbol obj)))

(define (genum->enum obj)
  (vtable-by-value (genum-class->value-table (class-of obj))
                   (genum->value obj)))

(define (genum->symbol obj)
  "Convert the enumerated value @var{obj} from a @code{<g-value>} to its
symbol representation (its ``nickname'')."
  (vtable-symbol (genum->enum obj)))

(define (genum->name obj)
  "Convert the enumerated value @var{obj} from a @code{<g-value>} to its
representation as a string (its ``name'')."
  (vtable-name (genum->enum obj)))

;;; flags

(define-class <g-flags-class> (<g-value-class>))
(define-class/docs <g-flags> (<g-value>)
  "A @code{<g-value>} base class for flag values. Users may define new
flag value types via subclssing from @code{<g-flags>}, passing
@code{#:vtable @var{table}} as an initarg, where @var{table} should be
in a format suitable for passing to @code{gflags-register-static}."
  #:g-type-name "GFlags"
  #:metaclass <g-flags-class>)

(define-method (initialize (class <g-flags-class>) initargs)
  (next-method class
               (supply-initarg-if-missing
                initargs #:g-type-name
                (lambda (initargs)
                  (let ((name (class-name->gtype-name (kw-ref initargs #:name))))
                    (gflags-register-static name (kw-ref initargs #:vtable))
                    name)))))

(define-method (write (obj <g-flags>) file)
  (format file "#<~a ~a ~a>"
          (class-name (class-of obj))
          (number->string (object-address obj) 16)
          (gflags->symbol-list obj)))

(define (gflags->element-list obj)
  (let ((vtable (gflags-class->value-table (class-of obj)))
        (value (gflags->value obj)))
    (filter (lambda (v)
              (= (logand value (vtable-index v)) (vtable-index v)))
            (vector->list vtable))))

(define (gflags->symbol-list obj)
  "Convert the flags value @var{obj} from a @code{<g-value>} to a list of
the symbols that it represents."
  (map vtable-symbol (gflags->element-list obj)))

(define (gflags->name-list obj)
  "Convert the flags value @var{obj} from a @code{<g-value>} to a list of
strings, the names of the values it represents."
  (map vtable-name (gflags->element-list obj)))

(define (gflags->value-list obj)
  "Convert the flags value @var{obj} from a @code{<g-value>} to a list of
integers, which when @code{logand}'d together yield the flags' value."
  (map vtable-index (gflags->element-list obj)))
