; The calculator in spirit.scheme

(define expression) ; forward declaration

(define expression
    (qi:>> (qi:int_)
        (qi:*
            (qi:|
                (qi:>> (qi:char_ "+") (qi:int_))
                (qi:>> (qi:char_ "-") (qi:int_))))))