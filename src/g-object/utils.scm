(define-module (g-object utils)
  #:use-module (g-object config)

  #:use-module (oop goops)

  #:export (g-type-name->scheme-name
            g-type-name->class-name))

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

(define (g-type-name->scheme-name type-name)
  "Transform a name of a @code{<gtype>}, such as \"GtkWindow\", to a
scheme form, such as @code{gtk-window}, taking into account the
exceptions in @code{g-type-name->scheme-name-alist}, and trimming
trailing dashes if any."
  (string-trim-right
   (camel-case->snake-case ;; only change _ to -, other characters are not valid in a type name
    (string-map (lambda (c) (if (eq? c #\_) #\- c)) type-name))
   #\-))

;; "GtkAccelGroup" => <gtk-accel-group>
;; "GSource*" => <g-source*>
(define (g-type-name->class-name type-name)
  "Transform a name of a @code{<gtype>}, such as \"GtkWindow\", to a
suitable name of a Scheme class, such as @code{<gtk-window>}. Uses
@code{g-type-name->scheme-name}."
  (string->symbol
   (string-append "<" (g-type-name->scheme-name type-name) ">")))
