(define-module (g-object g-signal)
  #:use-module (g-object config)
  #:use-module (g-object utils)

  #:use-module (g-object g-type)
  #:use-module (g-object g-closure)
  #:use-module (g-object g-value)

  #:use-module (oop goops)
  #:use-module (ice-9 optargs)

  #:export     (;; The signal class
                <g-signal>
                ;; Introspection
                gtype-class-get-signals gtype-class-get-signal-names
                ;; Emission
                gtype-instance-signal-emit
                ;; Connection, Disconnection, Blocking, Unblocking
                gtype-instance-signal-connect
                gtype-instance-signal-connect-after
                gsignal-handler-block gsignal-handler-unblock
                gsignal-handler-disconnect gsignal-handler-connected?
                ;; Creation
                gtype-class-create-signal))


(eval-when (expand load eval)
  (define-class/docs <g-signal> ()
    "A @code{<g-signal>} describes a signal on a @code{<gtype-instance>}:
its name, and how it should be called."
    (id #:init-keyword #:id #:init-value #f)
    (name #:init-keyword #:name)
    (interface-type #:init-keyword #:interface-type)
    (return-type #:init-keyword #:return-type)
    (param-types #:init-keyword #:param-types)
    (class-generic #:init-keyword #:class-generic #:init-value #f))

  (load-extension (extensions-library "gobject") "scm_gobject_gsignal_init"))


(define-method (initialize (instance <g-signal>) initargs)
  (next-method)
  (with-accessors
   (id name interface-type return-type param-types class-generic) instance
   (unless (id instance)
     (unless (class-generic instance)
       (set! (class-generic instance)
         (ensure-generic (lambda args (if #f #f))
                         (gtype-name->scheme-name (name instance)))))
     (set! (id instance)
       (gsignal-create
        instance
        (make <g-closure> #:func (class-generic instance)
              #:return-type (return-type instance)))))))

(define-method (write (obj <g-signal>) port)
  (with-accessors (name interface-type return-type param-types)
                  (format port "#<~a ~a ~a - ~a>"
                          (class-name (class-of obj)) (name obj) (return-type obj)
                          (cons (interface-type obj) (param-types obj)))))

(define (gtype-class-get-signal-names class)
  "Returns a vector of signal names belonging to @var{class} and all
parent classes."
  (with-accessors (name)
                  (map name (gtype-class-get-signals class))))

;;;
;;; {Emission}
;;;

;;;
;;; {Connecting, Disconnecting, Blocking, Unblocking}
;;;

(define* (gtype-instance-signal-connect object name func
                                        #:optional after? detail)
  "Connects @var{func} as handler for the @code{<gtype-instance>}
@var{object}'s signal @var{name}.

@var{name} should be a symbol. @var{after} is boolean specifying whether
the handler is run before (@code{#f}) or after (@code{#t}) the signal's
default handler, and @var{detail} is the optional \"detail\" parameter
to signal connection, which defaults to @code{#f}. @var{detail} is
mostly used when connecting to the @code{notify} signal of
@code{<gobject>}, in which case it should be the symbolic name of the
property whose change notifications you are interested in.

Returns an integer number which can be used as arugment of
@code{gsignal-handler-block}, @code{gsignal-handler-unblock},
@code{gsignal-handler-disconnect} and
@code{gsignal-handler-connected?}."
  (let ((signal (or (gsignal-query (class-of object) name)
                    (error "No such signal in class ~S: ~S"
                                    (class-of object) name))))
    (with-accessors (id return-type param-types)
                    (gtype-instance-signal-connect-closure
                     object (id signal)
                     (make <g-closure> #:func func #:return-type (return-type signal)
                           #:param-types (param-types signal))
                     after? detail))))

(define* (gtype-instance-signal-connect-after object name func
                                              #:optional detail)
  "Convenience function for calling
@code{gtype-instance-signal-connect} with @var{after} = @code{#t}."
  (gtype-instance-signal-connect object name func #t detail))

;;;
;;; {Creation and Definition}
;;;

;; fixme: unnecessary?
(define (gtype-class-create-signal class name return-type param-types)
  "Create a new signal associated with the @code{<g-type-class>}
@var{class}.

@var{name} should be a symbol, the name of the signal. @var{return-type}
should be a @code{<g-type-class>} object. @var{param-types} should be a
list of @code{<g-type-class>} objects.

In a bit of an odd interface, this function will return a new generic
function, which will be run as the signal's default handler, whose
default method will silently return an unspecified value. The user may
define new methods on this generic to provide alternative default
handler implementations."
  (with-accessors (class-generic)
                  (class-generic
                   (make <g-signal> #:name name #:interface-type class
                         #:return-type return-type #:param-types param-types))))
