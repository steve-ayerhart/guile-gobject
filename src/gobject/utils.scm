(define-module (gobject utils)
  #:use-module (gobject config)

  #:use-module (oop goops)

  #:export (gtype-name->scheme-name
            gtype-name->class-name
            class-name->gtype-name)

  #:export-syntax (define-class/docs with-accessors))

(eval-when (expand load eval)
  (define-syntax define-macro/docs
    (lambda (x)
      "Define a defmacro with documentation."
      (syntax-case x ()
        ((_ (f . args) doc b0 b* ...)
         #'(define-macro (f . args)
             doc b0 b* ...))))))

(define-macro/docs (define-class/docs name supers docs . rest)
  "Define a class named @var{name}, with superclasses @var{supers}, with
documentation @var{docs}."
  `(begin
     (define-class ,name ,supers ,@rest)
     (set-object-property! ,name 'documentation ,docs)))

(define-macro (with-accessors names . body)
  `(let (,@(map (lambda (name)
                  ;; Ew, fixme.
                  `(,name (make-procedure-with-setter
                           (lambda (x) (slot-ref x ',name))
                           (lambda (x y) (slot-set! x ',name y)))))
                names))
     ,@body))

(define (camel-case->snake-case nstr)
  "Expand the StudlyCaps @var{nstr} to a more schemey-form, according to
the conventions of GLib libraries. For example:
@lisp
 (GStudlyCapsExpand \"GSource\") @result{} gsource
 (GStudlyCapsExpand \"GtkIMContext\") @result{} gtk-im-context
 (GStudlyCapsExpand \"GtkHBox\") @result{} gtk-hbox
@end lisp"
  (do ((idx (+ -1 (string-length nstr)) (+ -1 idx)))
      ((> 1 idx) (string-downcase nstr))
    (cond ((and (> idx 2)
                (char-lower-case? (string-ref nstr (+ -3 idx)))
                (char-upper-case? (string-ref nstr (+ -2 idx)))
                (char-upper-case? (string-ref nstr (+ -1 idx)))
                (char-lower-case? (string-ref nstr idx)))
           (set! idx (1- idx))
           (set! nstr
             (string-append (substring nstr 0 (+ -1 idx))
                            "-"
                            (substring nstr (+ -1 idx)
                                       (string-length nstr)))))
          ((and (> idx 1)
                (char-upper-case? (string-ref nstr (+ -1 idx)))
                (char-lower-case? (string-ref nstr idx)))
           (set! nstr
             (string-append (substring nstr 0 (+ -1 idx))
                            "-"
                            (substring nstr (+ -1 idx)
                                       (string-length nstr)))))
          ((and (char-lower-case? (string-ref nstr (+ -1 idx)))
                (char-upper-case? (string-ref nstr idx)))
           (set! nstr
             (string-append (substring nstr 0 idx)
                            "-"
                            (substring nstr idx
                                       (string-length nstr))))))))

(define (gtype-name->scheme-name type-name)
  "Transform a name of a @code{<gtype>}, such as \"GtkWindow\", to a
scheme form, such as @code{gtk-window}, taking into account the
exceptions in @code{gtype-name->scheme-name-alist}, and trimming
trailing dashes if any."
  (string-trim-right
   (camel-case->snake-case ;; only change _ to -, other characters are not valid in a type name
    (string-map (lambda (c) (if (eq? c #\_) #\- c)) type-name))
   #\-))

;; "GtkAccelGroup" => <gtk-accel-group>
;; "GSource*" => <g-source*>
(define (gtype-name->class-name type-name)
  "Transform a name of a @code{<gtype>}, such as \"GtkWindow\", to a
suitable name of a Scheme class, such as @code{<gtk-window>}. Uses
@code{g-type-name->scheme-name}."
  (string->symbol
   (string-append "<" (gtype-name->scheme-name type-name) ">")))

(define (class-name->gtype-name class-name)
  "Convert the name of a class into a suitable name for a GType. For example:
@lisp
(class-name->gtype-name '<foo-bar>) @result{} \"FooBar\"
@endlisp"
  (list->string
   (reverse!
    (let loop ((to-process (string->list (symbol->string class-name)))
               (ret '())
               (caps? #t))
      (cond
       ((null? to-process)
        ret)
       ((char-alphabetic? (car to-process))
        (loop (cdr to-process)
              (cons (if caps? (char-upcase (car to-process)) (car to-process)) ret)
              #f))
       ((char-numeric? (car to-process))
        (loop (cdr to-process)
              (cons (car to-process) ret)
              #f))
       (else
        (loop (cdr to-process) ret #t)))))))
