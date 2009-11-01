extern package *keyword_pkg;
extern package *cl_pkg;
extern package *cl_user_pkg;

/*Keywords*/
//Symbol attributes
extern symbol *internal;
extern symbol *external;
extern symbol *inherited;
extern symbol *dynamic;
extern symbol *constant;
//Readtable attributes
extern symbol *constituent;
extern symbol *whitespace;
extern symbol *terminating_macro;
extern symbol *non_terminating_macro;
extern symbol *single_escape;
extern symbol *multiple_escape;
//Constituent traits
extern symbol *invalid;
extern symbol *alphabetic;
extern symbol *alphadigit;
extern symbol *package_marker;
extern symbol *plus_sign;
extern symbol *minus_sign;
extern symbol *dot;
extern symbol *decimal_point;
extern symbol *ratio_marker;
//Types
extern symbol *ttype;
extern symbol *constype;
extern symbol *fixnumtype;
extern symbol *bignumtype;
extern symbol *ratiotype;
extern symbol *singletype;
extern symbol *base_chartype;
extern symbol *vectortype;
extern symbol *arraytype;
extern symbol *compiled_functiontype;
extern symbol *stringtype;
extern symbol *symboltype;
extern symbol *functiontype;
extern symbol *hash_tabletype;
extern symbol *packagetype;
extern symbol *procinfotype;

/*Common-Lisp symbols*/
extern symbol *ts;//T symbol
extern symbol *nils;//NIL symbol
extern symbol *package_sym;//*package*
extern symbol *readtable;//*readtable*
//Lambda list control symbols
extern symbol *optional;//&optional
extern symbol *rest;//&rest
extern symbol *keyword;//&keyword
extern symbol *aux;//&aux
extern symbol *whole;//&whole
extern symbol *body;//&body
extern symbol *allow_other_keys;//&allow-other-keys
//Function symbols
extern symbol *cars;
extern symbol *quote;
