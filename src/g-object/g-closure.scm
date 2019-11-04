(define-module (g-object g-closure)
  #:use-module (g-object config)
  #:use-module (g-object utils)

  #:use-module (g-object g-type)
  #:use-module (g-object g-value)

  #:use-module (oop goops)

  #:export (<g-closure> gclosure-invoke))

(eval-when (expand load eval)
  (load-extension (extensions-library "gobject") "scm_gobject_gclosure_init"))

(define-class/docs <g-closure> (<g-boxed>)
  "The Scheme representation of a GLib closure: a typed procedure
object that can be passed to other languages." ;; FIXME say something about initargs
  #:g-type-name "GClosure")

;;;
;;; {Instance Allocation and Initialization}
;;;

(define-method (initialize (closure <g-closure>) initargs)
  ;; don't chain up, we do our own init
  (let ((return-type (get-keyword #:return-type initargs #f))
        (param-types (get-keyword #:param-types initargs '()))
        (func (get-keyword #:func initargs #f)))
    (gclosure-construct closure return-type param-types func)))
