(define-module (g-object config)
  #:export (extensions-library))

(define *gobjectlibdir*
  (cond ((getenv "GUILE_GOBJECT_BUILD_DIR")
         => (λ (builddir) (in-vicinity builddir ".libs")))
        (else "/usr/lib/guile/2.2/extensions")))

(define (extensions-library lib)
  (in-vicinity *gobjectlibdir* lib))
