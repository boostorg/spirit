blah ; blah not found

(define
  (foo x)
    (+ x 456))

(define
  (bar x)
    (+ x y)) ; y not found

(foo 123)
(foo z) ; z not found

(define foo 123) ; redefinition

(foo 123 456) ; incorrect arity

(bar 999) ; bar should not be found



