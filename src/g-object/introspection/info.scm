(define-module (g-object introspection info)
  #:use-module (g-object config)

  #:use-module (system foreign-object)
  #:use-module (oop goops)
  #:use-module (ice-9 match)
  #:use-module (rnrs enums)

  #:export (<gi-base-info>
            type namespace deprecated? name attributes
            <gi-callable-info>
            <gi-function-info>
            function-info->scm
            <gi-callback-info>
            <gi-value-info>
            <gi-signal-info>
            <gi-vfunc-info>
            <gi-property-info>
            <gi-field-info>
            <gi-arg-info>
            <gi-type-info>
            <gi-constant-info>
            <gi-registered-type-info>
            gtype type-name
            <gi-struct-info>
            <gi-enum-info>
            <gi-union-info>
            <gi-object-info>
            <gi-interface-info>))

(eval-when (expand eval load)
  (load-extension (extensions-library "gobject") "scm_c_gi_info_early_init")

  (define <gi-base-info>
    (make-foreign-object-type '<gi-base-info> '(info) #:finalizer %gi-base-info-finalizer))

  (define-class <gi-callable-info> (<gi-base-info>))
  (define-class <gi-callback-info> (<gi-callable-info>))
  (define-class <gi-function-info> (<gi-callable-info>))
  (define-class <gi-signal-info> (<gi-callable-info>))
  (define-class <gi-vfunc-info> (<gi-callable-info>))

  (define-class <gi-registered-type-info> (<gi-base-info>))
  (define-class <gi-enum-info> (<gi-registered-type-info>))
  (define-class <gi-interface-info> (<gi-registered-type-info>))
  (define-class <gi-object-info> (<gi-registered-type-info>))
  (define-class <gi-struct-info> (<gi-registered-type-info>))
  (define-class <gi-union-info> (<gi-registered-type-info>))

  (define-class <gi-arg-info> (<gi-base-info>))
  (define-class <gi-constant-info> (<gi-base-info>))
  (define-class <gi-field-info> (<gi-base-info>))
  (define-class <gi-property-info> (<gi-base-info>))
  (define-class <gi-type-info> (<gi-base-info>))
  (define-class <gi-value-info> (<gi-base-info>))

  (load-extension (extensions-library "gobject") "scm_c_gi_info_init"))

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

(define (gi-info-type-get-type n)
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
(define-method (type (info <gi-base-info>))
  (gi-info-type-get-type (%g-base-info-get-type info)))

(define-method (write (info <gi-base-info>) port)
  (format port "#<<~a-info> ~a>" (type info) (name info)))

(define-method (display (info <gi-base-info>) port)
  (format port "#<<~a-info> ~a>" (type info) (name info)))

(define-method (deprecated? (info <gi-base-info>))
  (%g-base-info-is-deprecated info))

(define-method (name (info <gi-base-info>))
  (%g-base-info-get-name info))

(define-method (namespace (info <gi-base-info>))
  (%g-base-info-get-namespace info))

(define-method (attributes (info <gi-base-info>))
  (%g-base-info-get-attributes info))

;;; registered type

(define-method (gtype (info <gi-registered-type-info>))
  (%g-registered-type-info-get-gtype info))

(define-method (type-name (info <gi-registered-type-info>))
  (%g-registered-type-info-get-type-name info))

;;; function

(define (function-info->scm info)
  (%gi-function-info->scm info))
