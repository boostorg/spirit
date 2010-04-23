(define (apply x f) (display (f x)))

(apply 5 (lambda (x) (* x 5)))

(display (quote (1 2 3 4 5)))

(display (quote ()))


(define for_each
    lambda (x f)
        f()


(define (display-all . args)
    (display args))

(display-all 123 456 999 666)

(display (+ 1 2 3 4 5 6))

(define (display-all first . rest) (display first) (display rest))

(display-all 123 456 999 666)


(define (display-all first . rest)
    (display first)
    (display (begin 1 2 rest)))

(display-all 123 456 999 666)


(define (foo x)
    (define (bar y z) (list x y z))
    (bar 9 (+ x 2)))

(display (foo 100))

; The hello-world for interpreters ;-)
(define (factorial n)
  (if (<= n 0) 1
    (* n (factorial (- n 1)))))

(display (factorial 10))



(define (foo x)
    (define (bar y z) (list x y z))
    (bar 9 (+ x 2)))

(define (main)
    (display (foo 100))) ; prints ( 100 9 102 )