(define-module (gobject gtype)
  #:use-module (gobject config)
  #:use-module (gobject utils)

  #:use-module (oop goops)

  #:export (<gtype-class> <gtype-instance> gtype-name->class gtype->class))

(eval-when (eval load compile)

  (define-class <gtype-class> (<class>)
    (gtype #:class <foreign-slot>))

  (define-class <gtype-instance> ()
    (instance #:class <read-only-slot>)
    #:gtype-name #t
    #:metaclass <gtype-class>)

  (load-extension (extensions-library "ggobject") "ggobject_gtype_init"))

(define-method (initialize (class <gtype-class>) initargs)
  (let ((gtype-name (or (get-keyword #:gtype-name initargs #f)
                        (error "Need #:gtype-name initarg: ~a" (pk initargs)))))
    (if (not (eq? gtype-name #t))
        (%gtype-class-bind class gtype-name))
    (next-method)
    (%gtype-class-inherit-magic class)))

(define-method (initialize (class <gtype-instance>) initargs)
  (%gtype-instance-initialize class initargs))
