/* init.c
 *
 * Daniel Ralston, 2009
 *
 * This file contains all code to set up a working Lisp environment.
 *
 * This code is released under the GNU GPL General Public License.
 */

#include "lisp.h"
#include "init.h"
#include "lbind.h"

#ifndef IN_OS
#include <stdlib.h>
#else
#include "malloc.h"
#endif

cons *basic_classes[20];

/* For initializaton, these don't need to be flexible. */
procinfo *proc;
cons *basic_env;

/* Packages */
package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/* Keywords */
/* Symbol attributes */
symbol *internal;
symbol *external;
symbol *inherited;
symbol *special;
symbol *constant;
/* Function Attributes */
symbol *special_operator;
/* Syntax Types */ 
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;
symbol *alphabetic;
symbol *alphadigit;
/* Constituent traits */
symbol *invalid;
symbol *package_marker;
symbol *plus_sign;
symbol *minus_sign;
symbol *dot;
symbol *decimal_point;
symbol *ratio_marker;

/* Common-Lisp symbols */
/* Types */
symbol *built_in_class_s;
symbol *number_s;
symbol *real_s;
symbol *rational_s;
symbol *integer_s;
symbol *fixnum_s;
symbol *bignum_s;
symbol *ratio_s;
symbol *complex_s;
symbol *float_s;
symbol *single_s;
symbol *character_s;
symbol *base_char_s;
symbol *extended_char_s;
symbol *sequence_s;
symbol *cons_s;
symbol *null_s;
symbol *vector_s;
symbol *array_s;
symbol *compiled_function_s;
symbol *string_s;
symbol *symbol_s;
symbol *function_s;
symbol *hash_table_s;
symbol *package_t_s;/* Symbol for the package type: not *PACKAGE*, but PACKAGE */
symbol *procinfo_s;
symbol *class_s;
symbol *stream_s;
/* Defined symbols */
symbol *t_s;/* T symbol */
symbol *nil_s;/* NIL symbol */
symbol *package_s;/*  *package*  */
symbol *readtable_s;/*  *readtable*  */
symbol *read_base_s;/*  *read-base*  */
/* Lambda list control symbols */
symbol *lambda_list_keywords_s;
symbol *optional_s;/* &optional */
symbol *rest_s;/* &rest */
symbol *keyword_s;/* &keyword */
symbol *aux_s;/* &aux */
symbol *whole_s;/* &whole */
symbol *body_s;/* &body */
symbol *allow_other_keys_s;/* &allow-other-keys */
/* Mathematic Function Names */
symbol *numberp_s;
/* List function names */
symbol *car_s;/* CAR symbol */
symbol *cdr_s;/* CDR symbol */
symbol *rplaca_s;
symbol *rplacd_s;
symbol *list_s;/* LIST symbol */
/* Special operators */
symbol *quote_s;/* QUOTE symbol */
/* Assignment Operator names */
symbol *intern_s;
symbol *defun_s;
symbol *setq_s;
/* Equality Function Names */
symbol *chareq_s;
symbol *charequal_s;
symbol *stringeq_s;
symbol *stringequal_s;
symbol *eq_s;
symbol *eql_s;
symbol *equal_s;
symbol *equalp_s;
/* Evaluation Function Names */
symbol *eval_s;
symbol *cond_s;
/* Environment Function Names */
symbol *find_package_s;
symbol *find_class_s;
/* Reader function names */
symbol *read_char_s;
symbol *read_s;
symbol *read_list_s;

/* Local variables */
symbol *value_s;
symbol *args_s;
symbol *object_s;
symbol *a_s;
symbol *b_s;
symbol *exp_s;

/*Local functions*/
symbol *initsym(char *name, package *p);
symbol *initintsym(char *name, package *p);
symbol *initcfun (char *name, 
		  cons *lambda_list, 
		  package *p, 
		  cons *(*fun)(cons *env));
cons *initdeftype(char *string);
cons *init_char_trait(char cc, cons *traitname, cons *traitval, cons *readtable);
cons *init_char_traits(char string[], cons *traitname, cons *traitval, cons *readtable);

void init_keyword_pkg();

void init_cl_pkg();
void init_readtable();
void init_lambda_control();
void init_special_operators();
void init_number_funs();
void init_list_funs();
void init_eq_funs();
void init_eval_funs();
void init_env_funs();
void init_read_funs();
void init_set_funs();
void init_types();

cons *initread(stream *str, cons *env);


/*Initialization helper functions*/
symbol *initsym(char *name, package *p)
{/* Initialize an external symbol named name in package p.
  */
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  if (p == keyword_pkg)
    {/* Keywords suffer from apartheid, as per CLHS: 
      */
      a->plist = fcons(fcons((cons*)external, t), 
		       fcons(fcons((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else
      a->plist = fcons(fcons((cons*)external, t), nil);
  return a;
}

symbol *initintsym(char *name, package *p)
{/*Initialize an internal symbol named name in package p.
  */
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  
  a->plist = fcons(fcons((cons*)internal, t), nil);
  return a;
}

symbol *initcfun (char *name, 
		  cons *lambda_list, 
		  package *p, 
		  cons *(*fun)(cons *env))
{/* Initialize a compiled C function in package p. 
  */
  symbol *funsym;
  compiled_function *f;

  funsym = intern(strtolstr(name), p);
  funsym->plist = fcons(fcons((cons*)external, t), 
			fcons(fcons((cons*)constant, t), nil));
  funsym->fun = (function*)newcompiled_function();
  f = (compiled_function*)funsym->fun;
  f->plist = fcons(fcons((cons*)external, t), 
		   fcons(fcons((cons*)constant, t), nil));
  f->env = basic_env;
  f->lambda_list = lambda_list;
  f->fun = fun;
  return funsym;
}

cons *initdeftype(char *string)
{
  stream *str = newstream(0);
  str->rv = strtolstr(string);
  cons *foo = initread(str, basic_env);
  foo->type = (cons*)BUILT_IN_CLASS;
  return foo;
}

cons *initread(stream *str, cons *env)
{/* An inflexible, incomplete implementation of read. Just what the programmer
  * needs to get the system off the ground, and not an iota more. To be extended
  * at the programmer's whim for convenience. Currently recognizes: lists, 
  * characters, symbols
  *
  * Stream support is rudimentary.
  */
  package *p = (package*)((procinfo*)env->car)->package_s->value;
  base_char *c = read_char(str);

  while (1)
    {
      if ((c == (base_char*)nil) || (c->c == ')'))
	return (cons*)0xbad;
      else if (c->c == ' ' || c->c == '\t')
	/* Skip over whitespace.
	 */
	c = read_char(str);
      else if (c->c == '(')
	{/* Read a list.
	  */
	  cons *foo = newcons();
	  cons *bar;
	  cons *to_ret = foo;

	  bar = (cons*)initread(str, env);
	  if (bar == (cons*)0xbad)
	    /*Recognize when the list is null.
	     */
	    return nil;
	  else
	    /* Otherwise, start reading a list!
	     */
	    foo->car = bar;
	  
	  while (1)
	    {/* Read a list, until a read returns nil, terminating the list.
	      */
	      bar = (cons*)initread(str, env);
	      if (bar == (cons*)0xbad)
		return to_ret;
	      foo->cdr = newcons();
	      foo = foo->cdr;
	      foo->car = bar;
	    }
	}
      else if (c->c >='0' && c->c <='9')
	/* If the token begins with a digit, parse a number.
	 */
	return 0;//TODO 
      else if (c->c == '#')
	{/* If the character is the macro sharpsign, interpret it.
	  */
	  c = read_char(str);
	  if (c->c == '\\')
	    {/* If #\ is the reader macro combination, read a character.
	      */
	      c = read_char(str);
	      return (cons*)ctolc(c->c);
	    }
	  else
	    c = read_char(str);
	}
      else
	{/* Otherwise, we are reading a symbol.
	  */
	  int i=0;
	  char name[100];
	  cons *foo=0;

	  while ((c->c != '(') &&
		 (c->c != ')') &&
		 (c->c != ' ') &&
		 (c->c != 0))
	    {/* Until we encounter something to terminate the symbol, read 
	      * characters into a buffer. Programmer: No symbols of more than 99
	      * characters if  you're using this function, and are too lazy to 
	      * modify it. (Safe to change, just an arbitrary buffer size.)
	      */
	      if (i >= 100)
		/* Even with the warning above, check to see if the programmer
		 * ignored my warnings. Prevents buffer overruns.
		 */
		return 0;//error

	      if ((c->c >= 'a') &&
		  (c->c <='z'))
		/* Convert the character to uppercase.
		 */
		c->c = c->c - 'A';

	      name[i] = c->c;
	      c = read_char(str);
	      i++;
	    }
	  unread_char(c, str);
	  if (i >= 100)
	    /* If we overrun the buffer, we are in error. return.
	     */
	    return 0;
	  else
	    name[i] = 0;
	  
	  if (c->c == 0)
	    /* If the character is the null character, unread it, or else
	     * successive reads won't know that the stream has ended.
	     */
	    unread_char(c, str);

	  if (name[0] == ':')
	    {
	      p = keyword_pkg;
	       return (cons*)intern(strtolstr(&name[1]), p);
	    }
	  
	  return (cons*)intern(strtolstr(name), p);
	}
    }
}

cons *init_char_trait(char cc, cons *traitname, cons *traitval, cons *readtable)
{
  base_char *c;
  cons *readtable_entry;
  cons *c_ppair;/* Character Property-Pair */
  cons *c_plist;/* Character Property-List */
  
  c = ctolc(cc);
  readtable_entry = assoc((cons*)c, readtable);
  c_plist = readtable_entry->cdr;
  
  readtable = setassoc((cons*)c, 
		       setassoc((cons*)traitname, traitval, c_plist),
		       readtable);
  return readtable;
}

cons *init_char_traits(char string[], cons *traitname, cons *traitval, cons *readtable)
{
  int i;

  for (i=0; string[i]!='\0'; i++)
    {
      readtable = init_char_trait(string[i], traitname, traitval, readtable);
    }
  return readtable;
}

/*Initialization*/
void init_keyword_pkg()
{/* Initialize keywords that we'll need to get things rolling.
  */
  array *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  /* External and constant must be initialized manually, because they depend on
   * themselves.
   */
  external = intern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = intern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(fcons((cons*)external, t), 
			  fcons(fcons((cons*)constant, t), nil));
  constant->plist = fcons(fcons((cons*)external, t), 
			  fcons(fcons((cons*)constant, t), nil));

  /* Other symbol attributes*/
  internal = initsym("INTERNAL", keyword_pkg);
  inherited = initsym("INHERITED", keyword_pkg);
  special = initsym("SPECIAL", keyword_pkg);

  special_operator = initsym("SPECIAL-OPERATOR", keyword_pkg);

  /* Readtable character attributes */
  constituent = initsym("CONSTITUENT", keyword_pkg);
  whitespace = initsym("WHITESPACE", keyword_pkg);
  terminating_macro = initsym("TERMINATING-MACRO-CHARACTER", keyword_pkg);
  non_terminating_macro = initsym("NON-TERMINATING-MACRO-CHARACTER", 
				  keyword_pkg);
  single_escape = initsym("SINGLE-ESCAPE", keyword_pkg);
  multiple_escape = initsym("MULTIPLE-ESCAPE", keyword_pkg);

  /* Consituent traits */
  invalid = initsym("INVALID", keyword_pkg);
  alphabetic = initsym("ALPHABETIC", keyword_pkg);
  alphadigit = initsym("ALPHADIGIT", keyword_pkg);
  package_marker = initsym("PACKAGE-MARKER", keyword_pkg);
  dot = initsym("DOT", keyword_pkg);
}

void init_cl_pkg()
{/* Initialize the Common Lisp package, and its contents.
  */
  array *cl_name = strtolstr("COMMON-LISP");
  cl_pkg = newpackage();
  cl_pkg->name = cl_name;

  /* Init T */
  t->type = (cons*)T;
  t_s = initsym("T", cl_pkg);
  t_s->value = t;
  
  /* Init NIL */
  nil->type = (cons*)LIST;
  nil->car = nil;
  nil->cdr = nil;
  nil_s = initsym("NIL", cl_pkg);

  nil_s->value = nil;

  package_s = initsym("*PACKAGE*", cl_pkg);
  package_s->value = (cons*)cl_pkg;
  proc->package_s = package_s;
  init_types();

  /* Generic paramter names */
  a_s = initintsym("A", cl_pkg);
  b_s = initintsym("B", cl_pkg);
  object_s = initintsym("OBJECT", cl_pkg);
  args_s = initintsym("ARGS", cl_pkg);
  exp_s = initintsym("EXP", cl_pkg);
  value_s = initintsym("VALUE", cl_pkg);

  /* Read-control variables */
  read_base_s = initsym("*READ-BASE*", cl_pkg);
  read_base_s->value = (cons*)newfixnum(10);

  init_lambda_control();
  init_number_funs();
  init_list_funs();
  init_eq_funs();
  init_read_funs();
  init_set_funs();
  init_special_operators();
}

void init_lambda_control()
{/* Initialize the lambda control characters for evalambda().
  */
  optional_s = initsym("&OPTIONAL", cl_pkg);
  rest_s = initsym("&REST", cl_pkg);
  keyword_s = initsym("&KEYWORD", cl_pkg);
  aux_s = initsym("&AUX", cl_pkg);
  whole_s = initsym("&WHOLE", cl_pkg);
  body_s = initsym("&BODY", cl_pkg);
  allow_other_keys_s = initsym("&ALLOW-OTHER-KEYS", cl_pkg);

  lambda_list_keywords_s = initsym("LAMBDA-LIST-KEYWORDS", cl_pkg);
  lambda_list_keywords_s->value = fcons((cons*)optional_s,
				      fcons((cons*)rest_s,
					    fcons((cons*)keyword_s,
						  fcons((cons*)aux_s,
							fcons((cons*)whole_s,
							      fcons((cons*)body_s,
								    fcons((cons*)allow_other_keys_s, nil)))))));
}

void init_list_funs()
{/* Initialize the list functions.
  */

  null_s = initcfun("NULL",
		    fcons((cons*)object_s,
			  nil),
		    cl_pkg,
		    &lnull);

  car_s = initcfun("CAR", 
		   fcons((cons*)list_s,
			 nil),
		   cl_pkg,
		   &lcar);
  cdr_s = initcfun("CDR", 
		   fcons((cons*)list_s,
			 nil),
		   cl_pkg,
		   &lcar);
  rplaca_s = initcfun("RPLACA",
		      fcons((cons*)cons_s,
			    fcons((cons*)a_s, 
				  nil)),
		      cl_pkg,
		      &lrplaca);
  rplacd_s = initcfun("RPLACD",
		      fcons((cons*)cons_s,
			    fcons((cons*)a_s,
				  nil)),
		      cl_pkg,
		      &lrplacd);
  list_s = initcfun("LIST",
		    fcons((cons*)rest_s, 
			  fcons((cons*)args_s,
				nil)),
		    cl_pkg,
		    &llist);
  cons_s = initcfun("CONS",
		    fcons((cons*)a_s,
			  fcons((cons*)b_s,
				nil)),
		    cl_pkg,
		    &lcons);
}

void init_number_funs()
{
  numberp_s = initcfun("NUMBERP",
		       fcons((cons*)object_s,
			     nil),
		       cl_pkg,
		       &lnumberp);
}

void init_eq_funs()
{/* Initialize the equality functions.
  */
  chareq_s = initcfun("CHAR=",
		     fcons((cons*)a_s, 
			   fcons((cons*)b_s,
				 nil)),
		     cl_pkg,
		     &lchareq);
  charequal_s = initcfun("CHAR-EQUAL",
			 fcons((cons*)a_s,
			       fcons((cons*)b_s,
				     nil)),
			cl_pkg,
			&lcharequal);

  stringeq_s = initcfun("STRING=",
		       fcons((cons*)a_s,
			     fcons((cons*)b_s,
				   nil)),
		       cl_pkg,
		       &lstringeq);
  
  stringequal_s = initcfun("STRING-EQUAL",
			  fcons((cons*)a_s,
				fcons((cons*)b_s, 
				      nil)),
			  cl_pkg,
			  &lstringequal);
  
  eq_s = initcfun("EQ", 
		 fcons((cons*)a_s, 
		       fcons((cons*)b_s,
			     nil)), 
		 cl_pkg, 
		 &leq);

  eql_s = initcfun("EQL",
		  fcons((cons*)a_s,
			fcons((cons*)b_s,
			      nil)),
		  cl_pkg,
		  &leql);
}

void init_eval_funs()
{
  eval_s = initcfun("EVAL",
		    fcons((cons*)exp_s,
			  nil),
		    cl_pkg,
		    &leval);

  cond_s = initcfun("COND",
		    fcons((cons*)intern(strtolstr("CLAUSES"), cl_pkg),
			  nil),
		    cl_pkg,
		    &lcond);
}

void init_env_funs()
{
  find_package_s = initcfun("FIND-PACKAGE",
			    fcons((cons*)string_s,
				  nil),
			    cl_pkg,
			    &lfind_package);

  find_class_s = initcfun("FIND-CLASS",
			  fcons((cons*)string_s,
				nil),
			  cl_pkg,
			  &lfind_class);
}

void init_special_operators()
{
  quote_s = initcfun("QUOTE", 
		     fcons((cons*)exp_s,
			   nil),
		     cl_pkg,
		     &lquote);
  quote_s->fun->plist = setassoc((cons*)special_operator, t, quote_s->plist);
}

void init_read_funs()
{/* Initialize reader functions.
  */
  base_char *c;
  cons *readtable_entry;
  cons *c_ppair;/* Character Property-Pair */
  cons *c_plist;/* Character Property-List */
  cons *readtable = nil;
  int i;
  int j;

  readtable_s = initsym("READTABLE", cl_pkg);

  stream *str = newstream(0);
  str->rv = strtolstr("(&OPTIONAL (STREAM *STANDARD-INPUT*) (EOF-ERROR-P T))");
  str->write_index = str->rv->length->num;

  read_char_s = initcfun("READ-CHAR",
			initread(str, basic_env),
			cl_pkg,
			&lread_char);
  read_list_s = initcfun("READ-LIST",
			 fcons((cons*)list_s,
			       fcons((cons*)character_s, nil)),
			 cl_pkg,
			 &lread_list);

  /* Syntax Types - CLHS 2.1.4 Fig. 2-7 */
  char whitespaces[] = "\t\n\r ";//Incomplete. What is a rubout? :S
  char constituents[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\b!$%&*+-./:<=>?@[]^_{}~";
  char t_macro_chars[] = "\"'(),;`";
  char n_t_macro_chars[] = "#";
  char s_escape_chars[] = "\\";
  char m_escape_chars[] = "|";
  
  /* Whitespaces and Constituents */
  readtable = init_char_traits(whitespaces, (cons*)whitespace, t, readtable);
  readtable = init_char_traits(constituents, (cons*)constituent, t, readtable);
  readtable = init_char_trait('.', (cons*)dot, t, readtable);
  /* Terminating Macro Characters */
  readtable = init_char_trait('(', (cons*)terminating_macro, (cons*)read_list_s, readtable);
  readtable = init_char_trait(')', (cons*)terminating_macro, t, readtable);
  //readtable = init_char_traits(t_macro_chars, (cons*)terminating_macro, (cons*)0, readtable);
  //readtable = init_char_traits(n_t_macro_chars, (cons*)non_terminating_macro, (cons*)0, readtable);
  /* Escape Characters */
  readtable = init_char_traits(s_escape_chars, (cons*)single_escape, t, readtable);
  readtable = init_char_traits(m_escape_chars, (cons*)multiple_escape, t, readtable);

  char invalids[] = "\b\t\n ";
  char alphabetics[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  char alphadigits[] = "0123456789";

  readtable = init_char_traits(invalids, (cons*)invalid, t, readtable);
  readtable = init_char_traits(alphabetics, (cons*)alphabetic, t, readtable);
  readtable = init_char_traits(alphadigits, (cons*)alphadigit, t, readtable);
  readtable = init_char_trait(':', (cons*)package_marker, t, readtable);

  readtable_s->value = readtable;
}

void init_set_funs()
{/* Initialize assignment functions.
  */

  intern_s = initcfun("INTERN",
		      fcons((cons*)string_s,
			    fcons((cons*)optional_s,
				  fcons((cons*)package_s,
					nil))),
		      cl_pkg,
		      &lintern);

  defun_s = initcfun("DEFUN",
		     fcons((cons*)intern(strtolstr("SYMBOL"), cl_pkg),
			   fcons((cons*)intern(strtolstr("LAMBDA-LIST"), cl_pkg),
				 fcons((cons*)intern(strtolstr("FORM"), cl_pkg),
				       nil))),
		     cl_pkg,
		     &ldefun);

  setq_s = initcfun("SETQ",
		    fcons((cons*)intern(strtolstr("SYMBOL"), cl_pkg),
			  fcons((cons*)intern(strtolstr("VALUE"), cl_pkg),
				nil)),
			
		    cl_pkg,
		    &lsetq);
}

void init_types()
{/* Initialize startup types.
  */

  t_s->class = initdeftype("(T () ())");
  basic_classes[T] = t_s->class;
  class_s = initsym("CLASS", cl_pkg);
  class_s->class = initdeftype("(CLASS (T) ())");
  basic_classes[CLASS] = class_s->class;
  built_in_class_s = initsym("BUILT-IN-CLASS", cl_pkg);
  built_in_class_s->class = initdeftype("(BUILT-IN-CLASS (CLASS) ())");
  basic_classes[BUILT_IN_CLASS] = built_in_class_s->class;

  number_s = initsym("NUMBER", cl_pkg);
  number_s->class = initdeftype("(NUMBER (T) ())");
  real_s = initsym("REAL", cl_pkg);
  real_s->class = initdeftype("(REAL (NUMBER) ())");

  rational_s = initsym("RATIONAL", cl_pkg);
  rational_s->class = initdeftype("(RATIONAL (REAL) ())");
  integer_s = initsym("INTEGER", cl_pkg);
  integer_s->class = initdeftype("(INTEGER (RATIONAL) ())");
  fixnum_s = initsym("FIXNUM", cl_pkg);
  fixnum_s->class = initdeftype("(FIXNUM (INTEGER) ())");
  basic_classes[FIXNUM] = fixnum_s->class;
  bignum_s = initsym("BIGNUM", cl_pkg);
  bignum_s->class = initdeftype("(BIGNUM (INTEGER) ())");
  basic_classes[BIGNUM] = bignum_s->class;
  ratio_s = initsym("RATIO", cl_pkg);
  ratio_s->class = initdeftype("(RATIO (RATIONAL) ())");
  basic_classes[RATIO] = ratio_s->class;

  float_s = initsym("FLOAT", cl_pkg);
  float_s->class = initdeftype("(FLOAT (REAL) ())");
  single_s = initsym("SINGLE-FLOAT", cl_pkg);
  single_s->class = initdeftype("(SINGLE-FLOAT (FLOAT) ())");
  basic_classes[SINGLE] = single_s->class;

  complex_s = initsym("COMPLEX", cl_pkg);
  complex_s->class = initdeftype("(COMPLEX (NUMBER) ())");
  
  sequence_s = initsym("SEQUENCE", cl_pkg);
  sequence_s->class = initdeftype("(SEQUENCE (T) ())");
  
  list_s = initsym("LIST", cl_pkg);
  list_s->class = initdeftype("(LIST (SEQUENCE) ())");
  basic_classes[LIST] = list_s->class;
  cons_s = initsym("CONS", cl_pkg);
  cons_s->class = initdeftype("(CONS (LIST) ())");
  null_s = initsym("NULL", cl_pkg);
  null_s->class = initdeftype("(NULL (SYMBOL LIST) ())");
  nil->type = null_s->class;

  array_s = initsym("ARRAY", cl_pkg);
  array_s->class = initdeftype("(ARRAY (T) ())");
  basic_classes[ARRAY] = array_s->class;
  vector_s = initsym("VECTOR", cl_pkg);
  vector_s->class = initdeftype("(VECTOR (ARRAY SEQUENCE) ())");
  basic_classes[VECTOR] = vector_s->class;
  string_s = initsym("STRING", cl_pkg);
  string_s->class = initdeftype("(STRING (VECTOR) ())");
  basic_classes[STRING] = string_s->class;

  character_s = initsym("CHARACTER", cl_pkg);
  character_s->class = initdeftype("(CHARACTER (T) ())");
  base_char_s = initsym("BASE-CHARACTER", cl_pkg);
  base_char_s->class = initdeftype("(BASE-CHARACTER (CHARACTER) ())");
  basic_classes[BASE_CHAR] = base_char_s->class;
  extended_char_s = initsym("EXTENDED-CHARACTER", cl_pkg);
  extended_char_s->class = initdeftype("(EXTENDED-CHARACTER (CHARACTER) ())");

  function_s = initsym("FUNCTION", cl_pkg);
  function_s->class = initdeftype("(FUNCTION (T) ())");
  basic_classes[FUNCTION] = function_s->class;
  compiled_function_s = initsym("COMPILED-FUNCTION", cl_pkg);
  compiled_function_s->class = initdeftype("(COMPILED-FUNCTION (FUNCTION) ())");
  basic_classes[COMPILED_FUNCTION] = compiled_function_s->class;

  symbol_s = initsym("SYMBOL", cl_pkg);
  symbol_s->class = initdeftype("(SYMBOL (T) ())");
  basic_classes[SYMBOL] = symbol_s->class;
  
  package_t_s = initsym("PACKAGE", cl_pkg);
  package_t_s->class = initdeftype("(PACKAGE (T) ())");
  basic_classes[PACKAGE] = package_t_s->class;

  procinfo_s = initintsym("PROCESS-INFO", cl_pkg);
  procinfo_s->class = initdeftype("(PROCESS-INFO (T) ())");
  basic_classes[PROCINFO] = procinfo_s->class;

  stream_s = initsym("STREAM", cl_pkg);
  stream_s->class = initdeftype("(STREAM (T) ())");
  basic_classes[STREAM] = stream_s->class;

  hash_table_s = initsym("HASH-TABLE", cl_pkg);
  hash_table_s->class = initdeftype("(HASH-TABLE (T) ())");
  basic_classes[HASH_TABLE] = hash_table_s->class;
}

procinfo *init()
{
  long i;
  long j;

  basic_env = extend_env(nil);

  /* Init process info */
  proc = malloc(sizeof(procinfo));
  proc->type = (cons*)PROCINFO;
  proc->packages = newcons();
  proc->packages->car = (cons*)cl_pkg;
  proc->packages->cdr = newcons();
  proc->packages->cdr->car = (cons*)cl_user_pkg;
  proc->packages->cdr->cdr = newcons();
  proc->packages->cdr->cdr->car = (cons*)keyword_pkg;  
  basic_env->car = (cons*)proc;

  init_keyword_pkg();
  init_cl_pkg();
  
  /* Init cl-user package */
  array *cl_user_name = strtolstr("COMMON_LISP_USER");
  cl_user_pkg = newpackage();
  cl_user_pkg->name = cl_user_name;

  return proc;
}
