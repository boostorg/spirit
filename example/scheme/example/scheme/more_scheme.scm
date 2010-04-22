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