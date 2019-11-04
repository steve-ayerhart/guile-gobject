(define-module (g-object g-type)
  #:use-module (g-object config)
  #:use-module (g-object utils)

  #:use-module (oop goops)

  #:export (<g-type-class> <g-type-instance> gtype-name->class gtype->class))

(eval-when (eval load compile)

  (define-class <g-type-class> (<class>)
    (g-type #:class <foreign-slot>))

  (define-class <g-type-instance> ()
    (instance #:class <read-only-slot>)
    #:g-type-name #t
    #:metaclass <g-type-class>)

  (load-extension (extensions-library "gobject") "scm_gobject_gtype_init"))

(define-method (initialize (class <g-type-class>) initargs)
  (let ((g-type-name (or (get-keyword #:g-type-name initargs #f)
                        (error "Need #:g-type-name initarg: ~a" (pk initargs)))))
    (if (not (eq? g-type-name #t))
        (%gtype-class-bind class g-type-name))
    (next-method)
    (%gtype-class-inherit-magic class)))

(define-method (initialize (class <g-type-instance>) initargs)
  (%gtype-instance-initialize class initargs))
