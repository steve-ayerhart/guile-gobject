(define-module (g-object config)
  #:export (extensions-library))

(define *ggobjectlibdir*
  (cond ((getenv "GGOBJECT_BUILD_DIR")
         => (λ (builddir) (in-vicinity builddir ".libs")))
        (else "/usr/lib/guile/2.2/extensions")))

(define (extensions-library lib)
  (in-vicinity *ggobjectlibdir* lib))
