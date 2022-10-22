#lang racket

(provide parse)

(define char-symbolic?
  (lambda (char)
      (and (not (char-whitespace? char))
               (not (eq? char #\())
	                (not (eq? char #\))))))

(define char->number
  (lambda (char)
      (- (char->integer char)
             (char->integer #\0))
	         )
		   )

(define parse
  (lambda (str)
      (car (E (string->list str)))
          ))

(define E
  (lambda (input)
      (cond
            [(equal? (car input) #\( ) (L (cdr input) '())]

      [(D? (car input)) (let* ((d-val (D input))
                                     (d-ast (car d-val))
				                                    (n-val (N (cdr d-val) d-ast))
								                                   (n-ast (car n-val)))

                        (cons n-ast (cdr n-val)))]

      [(A? (car input)) (let* ((a-val (A input))
                                     (a-ast (car a-val))
				                                    (s-val (S (cdr a-val) a-ast))
								                                   (s-ast (car s-val)))

                        (cons s-ast (cdr s-val)))]
			    )))

(define L
  (lambda (input lhs)
      (cond
            [(null? input) (cons lhs null)]

      [(equal? (car input) #\) ) (cons lhs (cdr input))]

      [(char-whitespace? (car input)) (L (cdr input) lhs)]

      [else (let* ((e-val (E input))
                         (e-ast (car e-val))
			                    (l-val (L (cdr e-val) null))
					                       (l-ast (car l-val)))

                  (cons (cons e-ast l-ast) (cdr l-val)))]
		        )))

(define S
  (lambda (input lhs)
        (cond
	        [(null? input) (cons (string->symbol lhs) null)]

        [(A? (car input)) (let* ((a-val (A input))
	                               (a-ast (car a-val))
				                                      (s-val (S (cdr a-val) (string-append lhs a-ast)))
								                                     (s-ast (car s-val)))

               (cons s-ast (cdr s-val)))]

        [else (cons (string->symbol lhs) input)]
	        )))

(define N
  (lambda (input lhs)
      (cond [(null? input) (cons lhs null)]

          [(D? (car input)) (let* ((d-val (D input))
	                                     (d-ast (car d-val))
					                                        (n-val (N (cdr d-val)
										                                             (+ (* 10 lhs) d-ast)))
															                                        (n-ast (car n-val)))

                            (cons n-ast (cdr n-val)))]

          [else (cons lhs input)]
	            )))

(define D
  (lambda (input)
      (cond
            [(D? (car input)) (cons (char->number (car input)) (cdr input))]
	          [else (writeln "Error")]
		        )))

(define D? char-numeric?)

(define A
  (lambda (input)
      (cond
            [(A? (car input)) (cons (string (car input)) (cdr input))]

      [else (writeln "Error")]
            )))

(define A?
  (lambda (input)
      (and (not (char-numeric? input)) (char-symbolic? input))
            ))
	    