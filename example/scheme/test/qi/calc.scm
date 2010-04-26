; The calculator in spirit.scheme

(define expression) ; forward declaration

(define expression
    (qi:>> (qi:int_)
        (qi:*
            (qi:|
                (qi:>> (qi:char_ "+") (qi:int_))
                (qi:>> (qi:char_ "-") (qi:int_))))))

; (define expression)

; (define factor
    ; (qi:|
        ; (qi:int_)
        ; (qi:>> (qi:char_ "(") (expression) (qi:char_ ")"))
        ; (qi:>> (qi:char_ "-") (factor))
        ; (qi:>> (qi:char_ "+") (factor))))

; (define term
    ; (qi:>> (factor)
        ; (qi:*
            ; (qi:|
                ; (qi:>> (qi:char_ "*") (factor))
                ; (qi:>> (qi:char_ "/") (factor))))))

; (define expression
    ; (qi:>> (term)
        ; (qi:*
            ; (qi:|
                ; (qi:>> (qi:char_ "+") (term))
                ; (qi:>> (qi:char_ "-") (term))))))