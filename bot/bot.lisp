(ql:quickload 'postmodern)

(use-package 'postmodern)

(defclass fx-position ()
  ((major :initarg :major :col-type (string 3))
   (minor :initarg :minor :col-type (string 3))
   (value :initarg :value :col-type double-float))
  (:metaclass dao-class))

(deftable fx-position
    (!dao-def))

(connect-toplevel "fxdb" "bot" "PASSWORD" "localhost")

; ------------------------------------------------------
; Initialize the database if tables don't exist.
; ------------------------------------------------------

(if (null
     (query (:select '* :from 'pg_tables 
		     :where (:= 'tablename "fx_position"))))
    (create-table 'fx-position))

;; more stuff here.

