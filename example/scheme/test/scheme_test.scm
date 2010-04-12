(define (dbl x) (+ x x))

(define len 123)

(dbl len) ; 246

; The hello-world for interpreters ;-)
(define (factorial n)
    (if (<= n 0) 1
        (* n (factorial (- n 1)))))

(factorial 10) ; 3628800