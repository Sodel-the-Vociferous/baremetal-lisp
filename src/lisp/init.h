#define CASE_DIFFERENCE = 32
extern cons *basic_classes[15];

/*Packages*/
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
//Syntax Types
extern symbol *constituent;
extern symbol *whitespace;
extern symbol *terminating_macro;
extern symbol *non_terminating_macro;
extern symbol *single_escape;
extern symbol *multiple_escape;
extern symbol *alphabetic;
extern symbol *alphadigit;
//Constituent traits
extern symbol *invalid;
extern symbol *package_marker;
extern symbol *plus_sign;
extern symbol *minus_sign;
extern symbol *dot;
extern symbol *decimal_point;
extern symbol *ratio_marker;

/*Common-Lisp symbols*/
//Internal symbols, for me.
extern symbol *special_operators_s;
//Types
extern symbol *built_in_class_s;
extern symbol *number_s;
extern symbol *real_s;
extern symbol *rational_s;//rational
extern symbol *integer_s;
extern symbol *fixnum_s;
extern symbol *bignum_s;
extern symbol *ratio_s;
extern symbol *float_s;
extern symbol *single_s;
extern symbol *character_s;
extern symbol *base_char_s;
extern symbol *extended_character_s;
extern symbol *sequence_s;
extern symbol *cons_s;
extern symbol *null_s;
extern symbol *vector_s;
extern symbol *array_s;
extern symbol *compiled_function_s;
extern symbol *string_s;
extern symbol *symbol_s;
extern symbol *function_s;
extern symbol *hash_table_s;
extern symbol *package_s;
extern symbol *procinfo_s;
extern symbol *class_s;
//Defined symbols
extern symbol *t_s;//T symbol
extern symbol *nil_s;//NIL symbol
extern symbol *package_s;//*package*
extern symbol *readtable_s;//*readtable*
/* Lambda list control symbols */
extern symbol *lambda_list_keywords_s;
extern symbol *optional_s;//&optional
extern symbol *rest_s;//&rest
extern symbol *keyword_s;//&keyword
extern symbol *aux_s;//&aux
extern symbol *whole_s;//&whole
extern symbol *body_s;//&body
extern symbol *allow_other_keys_s;//&allow-other-keys
//List function names
extern symbol *car_s;//CAR symbol
extern symbol *cdr_s;//CDR symbol
extern symbol *list_s;//LIST symbol
//Special operators
extern symbol *quote_s;//QUOTE symbol
//Equality function names
extern symbol *chareq_s;
extern symbol *charequal_s;
extern symbol *stringeq_s;
extern symbol *stringequal_s;
extern symbol *eq_s;
extern symbol *eql_s;
extern symbol *equal_s;
extern symbol *equalp_s;
//Reader function names
extern symbol *read_char_s;
extern symbol *read_s;
