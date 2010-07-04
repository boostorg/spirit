(define (foo x)
    (define (bar y z) (list x y z))
    (bar 9 x))

(display (foo 100))