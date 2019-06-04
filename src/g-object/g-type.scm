(define-module (g-object g-type)
  #:use-module (g-object config)

  #:use-module (oop goops))

(eval-when (eval load compile)
  (dynamic-call "scmg_type_init"
                (dynamic-link (extensions-library "ggobject"))))

(define-class <g-type> ()
  #:metaclass <g-type-class>)

(define-method (allocate-instance (g-type-class <g-type-class>) initargs)
  (%allocate-instance g-type-class))

(define-method (initialize (g-type-instance <g-type>) . initargs)
  (%initialize g-type-instance initargs))

(define-method (make-instance (g-type-class <g-type-class>) . initargs)
  (let ((instance (allocate-instance g-type-class initargs)))
    (initialize instance initargs)
    instance))

