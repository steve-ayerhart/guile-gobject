(define-module (g-object introspection repository)
  #:use-module (g-object config)
  #:use-module (g-object introspection info)

  #:use-module (system foreign-object)
  #:use-module (oop goops)
  #:export (gi-repository-require
            gi-repository-get-infos
            gi-repository-find-by-name))

(eval-when (expand eval load)
  (load-extension (extensions-library "gobject") "scm_c_gi_repository_init"))
