(define-module (g-object introspection module)
  #:use-module (g-object config)
  #:use-module (g-object utils)
  #:use-module (g-object introspection info)
  #:use-module (g-object introspection repository)

  #:use-module (oop goops))

(define-method (gi-import (module <module>) (name <symbol>))
  (let* ((namespace (object-property module 'namespace))
         (info (gi-repository-find-by-name namespace name)))

    (cond
     ((is-a? info <gi-enum-info>)
      module)
     ((is-a? info <gi-constant-info>)
      (module-define! module
                      ((compose string->symbol gtype-name->scheme-name symbol->string) name)
                      (value info))
      (module-export-all! module)
      (module-use! (current-module) (resolve-module (module-name module)))))))
