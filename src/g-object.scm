(define-module (g-object)
  #:use-module (g-object config)
  #:use-module (oop goops))

(define-class <g-object-class> (<class>)
  (g-type-name #:init-keyword #:g-type-name))

(eval-when (eval load compile)
  (dynamic-call "ggobject_init"
                (dynamic-link (extensions-library "ggobject"))))
