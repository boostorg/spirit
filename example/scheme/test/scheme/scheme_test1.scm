; These tests demostrate the functionality of the scheme
; compiler/interpreter

(define (dbl x) (+ x x))

(define len 123)

(dbl len) ; 246

; The hello-world for interpreters ;-)
(define (factorial n)
  (if (<= n 0) 1
    (* n (factorial (- n 1))))

(factorial 10) ; 3628800

; Fibonacci using lambda
(define fib
  (lambda (n)
    (if (< n 2)
        n
        (+ (fib (- n 1)) (fib (- n 2))))))

(fib 10) ; 55