/* init.h
 *
 * Daniel Ralston, 2009
 *
 * This code is released under the GNU GPL General Public License.
 */
#ifndef INIT_H
#define INIT_H

#define CASE_DIFFERENCE = 32

cons *basic_classes[20];

/* For initializaton, these don't need to be flexible. */
cons *basic_env;

/* Packages */
package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/* Keywords */
extern symbol *readtable_case;
extern symbol *upcase;
extern symbol *downcase;
extern symbol *preserve;
extern symbol *invert;
/* Symbol attributes */
extern symbol *internal;
extern symbol *external;
extern symbol *inherited;
extern symbol *special;
extern symbol *constant;
/* Function Attributes */
extern symbol *special_operator;
/* Syntax Types */ 
extern symbol *constituent;
extern symbol *whitespace;
extern symbol *terminating_macro;
extern symbol *non_terminating_macro;
extern symbol *single_escape;
extern symbol *multiple_escape;
extern symbol *alphabetic;
extern symbol *alphadigit;
/* Constituent traits */
extern symbol *invalid;
extern symbol *package_marker;
extern symbol *plus_sign;
extern symbol *minus_sign;
extern symbol *dot;
extern symbol *decimal_point;
extern symbol *ratio_marker;

/* Common-Lisp symbols */
/* Types */
extern symbol *built_in_class_s;
extern symbol *number_s;
extern symbol *real_s;
extern symbol *rational_s;
extern symbol *integer_s;
extern symbol *fixnum_s;
extern symbol *bignum_s;
extern symbol *ratio_s;
extern symbol *complex_s;
extern symbol *float_s;
extern symbol *single_s;
extern symbol *character_s;
extern symbol *base_char_s;
extern symbol *extended_char_s;
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
extern symbol *package_t_s;/* Symbol for the package type: not *PACKAGE*, but PACKAGE */
extern symbol *procinfo_s;
extern symbol *class_s;
extern symbol *stream_s;
/* Defined symbols */
extern symbol *t_s;/* T extern symbol */
extern symbol *nil_s;/* NIL extern symbol */
extern symbol *package_s;/*  *package*  */
extern symbol *readtable_s;/*  *readtable*  */
extern symbol *read_base_s;/*  *read-base*  */
/* Lambda list control symbols */
extern symbol *lambda_list_keywords_s;
extern symbol *optional_s;/* &optional */
extern symbol *rest_s;/* &rest */
extern symbol *keyword_s;/* &keyword */
extern symbol *aux_s;/* &aux */
extern symbol *whole_s;/* &whole */
extern symbol *body_s;/* &body */
extern symbol *allow_other_keys_s;/* &allow-other-keys */
/* Mathematic Function Names */
extern symbol *numberp_s;
extern symbol *plus_sign_s;
extern symbol *minus_sign_s;
extern symbol *asterisk_s;
/* List function names */
extern symbol *car_s;/* CAR extern symbol */
extern symbol *cdr_s;/* CDR extern symbol */
extern symbol *rplaca_s;
extern symbol *rplacd_s;
extern symbol *list_s;/* LIST extern symbol */
/* Special operators */
extern symbol *quote_s;/* QUOTE extern symbol */
/* Assignment Operator names */
extern symbol *defun_s;
extern symbol *setq_s;
extern symbol *let_s;
extern symbol *let_star_s;
extern symbol *defparameter_s;
extern symbol *defvar_s;
/* Equality Function Names */
extern symbol *chareq_s;
extern symbol *charequal_s;
extern symbol *stringeq_s;
extern symbol *stringequal_s;
extern symbol *eq_s;
extern symbol *eql_s;
extern symbol *equal_s;
extern symbol *equalp_s;
/* Evaluation Function Names */
extern symbol *progn_s;
extern symbol *eval_s;
extern symbol *cond_s;
extern symbol *type_of_s;
extern symbol *documentation_s;
/* Environment Function Names */
extern symbol *intern_s;
extern symbol *find_package_s;
extern symbol *find_class_s;
/* Reader function names */
extern symbol *read_char_s;
extern symbol *read_s;
extern symbol *read_list_s;
/* Internal Symbols */
extern symbol *variable_documentation_s;
/* Local variables */
extern symbol *value_s;
extern symbol *args_s;
extern symbol *object_s;
extern symbol *a_s;
extern symbol *b_s;
extern symbol *exp_s;
extern symbol *clauses_s;
extern symbol *name_s;
#endif
