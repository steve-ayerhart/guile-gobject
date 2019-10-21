(define-module (gobject introspection info)
  #:use-module (gobject config)
  #:use-module (system foreign-object)
  #:use-module (oop goops)
  #:use-module (ice-9 match)
  #:use-module (rnrs enums)

  #:export (<base-info>
            type namespace deprecated? name attributes
            <callable-info>
            <function-info>
            <callback-info>
            <value-info>
            <signal-info>
            <vfunc-info>
            <property-info>
            <field-info>
            <arg-info>
            <type-info>
            <constant-info>
            <registered-type-info>
            gtype type-name
            <struct-info>
            <enum-info>
            <union-info>
            <object-info>
            <interface-info>))

(eval-when (expand eval load)
  (load-extension (extensions-library "ggobject") "ggi_info_early_init")

  (define <base-info>
    (make-foreign-object-type '<base-info> '(info) #:finalizer %ggi-finalize-info))

  (define-class <callable-info> (<base-info>))
  (define-class <callback-info> (<callable-info>))
  (define-class <function-info> (<callable-info>))
  (define-class <signal-info> (<callable-info>))
  (define-class <vfunc-info> (<callable-info>))

  (define-class <registered-type-info> (<base-info>))
  (define-class <enum-info> (<registered-type-info>))
  (define-class <interface-info> (<registered-type-info>))
  (define-class <object-info> (<registered-type-info>))
  (define-class <struct-info> (<registered-type-info>))
  (define-class <union-info> (<registered-type-info>))

  (define-class <arg-info> (<base-info>))
  (define-class <constant-info> (<base-info>))
  (define-class <field-info> (<base-info>))
  (define-class <property-info> (<base-info>))
  (define-class <type-info> (<base-info>))
  (define-class <value-info> (<base-info>))

  (load-extension (extensions-library "ggobject") "ggi_info_init"))

(define gi-info-type-enum
  (make-enumeration
   '(invalid
     function
     callback
     struct
     boxed
     enum ; 5
     flags
     object
     interface
     constant
     invalid-0 ; 10
     union
     value
     signal
     vfunc
     property ; 15
     field
     arg
     type
     unresolved)))
(define gi-info-type-index
  (enum-set-indexer gi-info-type-enum))

(define (gi-info-type-get n)
  (match n
    (0 'invalid)
    (1 'function)
    (2 'callback)
    (3 'struct)
    (4 'boxed)
    (5 'enum)
    (6 'flags)
    (7 'object)
    (8 'interface)
    (9 'constant)
    (10 'invalid-0)
    (11 'union)
    (12 'value)
    (13 'signal)
    (14 'vfunc)
    (15 'property)
    (16 'field)
    (17 'arg)
    (18 'type)
    (19 'unresolved)))

;;; base
(define-method (type (info <base-info>))
  (gi-info-type-get (%ggi-base-info-get-type info)))

(define-method (write (info <base-info>) port)
  (format port "#<<~a-info> ~a>" (type info) (name info)))

(define-method (display (info <base-info>) port)
  (format port "#<<~a-info> ~a>" (type info) (name info)))

(define-method (deprecated? (info <base-info>))
  (%ggi-base-info-is-deprecated info))

(define-method (name (info <base-info>))
  (%ggi-base-info-get-name info))

(define-method (namespace (info <base-info>))
  (%ggi-base-info-get-namespace info))

(define-method (attributes (info <base-info>))
  (%ggi-base-info-get-attributes info))

;;; registered type

(define-method (gtype (info <registered-type-info>))
  (%ggi-registered-type-info-get-gtype info))

(define-method (type-name (info <registered-type-info>))
  (%ggi-registered-type-info-get-type-name info))
