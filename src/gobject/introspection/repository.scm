(define-module (gobject introspection repository)
  #:use-module (gobject config)
  #:use-module (gobject introspection info)
  #:use-module (system foreign-object)
  #:use-module (oop goops)
  #:export (repository-require
            repository-get-infos))

(eval-when (eval load)
  (load-extension (extensions-library "ggobject") "ggi_repository_init"))
