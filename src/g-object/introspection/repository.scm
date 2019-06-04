(define-module (g-object introspection repository)
  #:use-module (g-object config)
  #:use-module (g-object)
  #:use-module (oop goops))

(eval-when (eval load compile)
  (dynamic-call "scmgi_repository_init"
                (dynamic-link (extensions-library "ggobject"))))

;;; base
(define-method (namespace (info <info>))
  (%info-get-namespace info))

(define-method (deprecated? (info <info>))
  (%info-is-deprecated info))

(define-method (name (info <info>))
  (%info-get-name info))

(define-method (write (info <info>) port)
  (%info-display info port))

(define-method (display (info <info>) port)
  (%info-display info port))

(define-method (namespace (info <info>))
  (%info-get-namespace info))

(define-method (depcrecated? (info <info>))
  (%info-is-deprecated info))

;;; registered type
(define-class <registered-type-info> (<info>))

(define-method (registered-type-info? (info <info>))
  (%registered-type-info? info))

(define-class <function-info> (<info>))
(define-class <callback-info> (<info>))
(define-class <boxed-info> (<info>))
(define-class <flags-info> (<info>))
(define-class <value-info> (<info>))
(define-class <signal-info> (<info>))
(define-class <v-func-info> (<info>))
(define-class <property-info> (<info>))
(define-class <field-info> (<info>))
(define-class <arg-info> (<info>))
(define-class <type-info> (<info>))
(define-class <struct-info> (<registered-type-info>))
(define-class <enum-info> (<registered-type-info>))

;;; object
(define-class <object-info> (<registered-type-info>))

(define-method (object-info? (info <info>))
  (%object-info? info))

(define-method (methods (object-info <object-info>))
  (%object-info-get-methods object-info))

;;; interface
(define-class <interface-info> (<registered-type-info>))

;;; constant
(define-class <constant-info> (<info>))
(define-method (value (info <constant-info>))
  (%constant-value info))

(define-class <union-info> (<registered-type-info>))


(define-method (arg-info? (info <info>))
  (%arg-info? info))

(define-method (callable-info? (info <info>))
  (%callable-info? info))
