#lang racket
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CSCI 301, Winter 2021
;;
;; Lab #7
;;
;; Hunter Smith
;; W01495037
;;
;; The purpose of this program is to interpret input
;;
;;
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(provide lookup
         evaluate
	          special-form?
		           evaluate-special-form
				 )

(define closure (lambda (vars body env) (mcons 'closure (mcons env (mcons vars body)))))
(define closure? (lambda (clos) (and (mpair? clos) (eq? (mcar clos) 'closure))))
(define closure-env (lambda (clos) (mcar (mcdr clos))))
(define closure-vars (lambda (clos) (mcar (mcdr (mcdr clos)))))
(define closure-body (lambda (clos) (mcdr (mcdr (mcdr clos)))))
(define set-closure-env!(lambda (clos new-env) (set-mcar! (mcdr clos) new-env)))

(define special-form?
  (lambda (list)
      (cond
            [(not (list? list)) #f]

      [(equal? (car list) 'if) #t]

      [(equal? (car list) 'cond) #t]

      [(equal? (car list) 'let) #t]

      [(equal? (car list) 'letrec) #t]

      [(equal? (car list) 'lambda) #t]

      [(and (list? (car list)) (equal? (caar list) 'lambda)) #t]

      [else #f]
            )))

(define lookup
  (lambda (symb envr)
        (cond
	        [(equal? (caar envr) symb) (cadar envr)]

        [else
	             (lookup symb (cdr envr))]
		           )))

(define evaluate
  (lambda (exp envr)
        (cond
	        [(closure? exp) exp]

        [(number? exp) exp]

        [(symbol? exp) (lookup exp envr)]

        [(special-form? exp) (evaluate-special-form exp envr)]

        [(list? exp)
	          (apply-function (evaluate (car exp) envr) (map (lambda (num)

          (evaluate num envr))
	  	                (cdr exp)))]
				        )))

(define apply-function
  (lambda (func args)
      (cond
           [(procedure? func) (apply func args)]

     [(closure? func) (apply-closure func args)]
          )))

(define apply-closure
  (lambda (clos vals)
      (evaluate (closure-body clos) (append (map list (closure-vars clos) vals) (closure-env clos)))
          ))

(define evaluate-special-form
  (lambda (exp envr)
      (cond
            [(equal? (car exp) 'if)
	           (if
		           (evaluate (cadr exp) envr)

        (evaluate (caddr exp) envr)

        (evaluate (cadddr exp) envr)
	       )]

      [(equal? (car exp) 'cond)
             (evaluate-cond (cdr exp) envr) ]

      [(equal? (car exp) 'let)

       (define temp-envr
                (map list (map (lambda (item) (car item)) (cadr exp))

                   (map (lambda (item) (evaluate (cadr item) envr)) (cadr exp)
		          )))
			         (evaluate (caddr exp) (append temp-envr envr))]

      [(equal? (car exp) 'letrec)

       (define Minienvr
                (map list (map (lambda (item) (car item)) (cadr exp))

                   (map (lambda (item) (evaluate (cadr item) envr)) (cadr exp)
		          )))

       (define Newenvr
                (append Minienvr envr))

       (fixenvr Minienvr Newenvr)

       (evaluate (caddr exp) Newenvr)]

      [(equal? (car exp) 'lambda)
             (evaluate (closure (cadr exp) (caddr exp) envr) envr)]

      [(equal? (caar exp) 'lambda)
              (evaluate (caddar exp) (append (map list (cadar exp) (map (lambda (vals) (evaluate vals envr)) (cdr exp))) envr))]
	          )))

(define fixenvr
  (lambda (Minienvr Newenvr)
          (cond
	        [(closure? (cadar Minienvr)) (set-closure-env! (cadar Minienvr) Newenvr)]

      [else (fixenvr (cdr Minienvr) Newenvr)]
            )
	      ))

(define evaluate-cond
  (lambda (exp envr)
      (cond
            [(evaluate (caar exp) envr) (evaluate (cadar exp) envr)]

      [else (evaluate-cond (cdr exp) envr)]
          )))