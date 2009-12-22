(defun interpret-token (token)
  ())

(defun read-token (input-stream character &optional (multiple-escaped nil))
  ())

(defun read (&optional (input-stream *standard-input*) (eof-error-p t) eof-value recursive-p)
  (let* ((c (read-char input-stream nil nil))
	 (type-list (gethash c *readtable*))
	 (type (car type-list)))
    (cond ((eq c nil) nil)
	  ((eq type :invalid) nil);TODO reader error
	  ((eq type :whitespace) (read input-stream 
				       eof-error-p
				       eof-value
				       recursive-p))
	  ((or (eq type :terminating-macro-character)
	       (eq type :non-terminating-macro-character) (apply (cdr type-list) 
								 input-stream 
								 c)))
	  ((eq type :single-escape) (progn (setq c 
						 (read-char input-stream))
					   (interpret-token (read-token input-stream 
									c))))
	  ((eq type :multiple-escape) (progn (setq c (read-char input-stream))
					     (interpret-token (read-token input-stream 
									  c 
									  t))))
	  ((eq type :constituent) (interpret-token (read-token input-stream 
							       c)))
	  (t nil))))