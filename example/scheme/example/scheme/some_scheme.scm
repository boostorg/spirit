(define (apply x f) (display (f x)))

(apply 123 (lambda (x) (+ x 1)))