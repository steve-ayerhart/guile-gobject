(define-module (g-object introspection repository)
  #:use-module (g-object config)
  #:use-module (oop goops))

(eval-when (eval load compile)
  (dynamic-call "scmggi_repository_init"
                (dynamic-link (extensions-library "ggobject"))))
