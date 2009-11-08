#include <stdlib.h>
#include "lisp.h"
#include "init.h"
#include "lbind.h"

/*For initializaton, these don't need to be flexible.*/
procinfo *proc;
cons *basic_env;

/*Packages*/
package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/*Keywords*/
//Symbol attributes
symbol *internal;
symbol *external;
symbol *inherited;
symbol *dynamic;
symbol *constant;
//Syntax Types
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;
symbol *alphabetic;
symbol *alphadigit;
//Constituent traits
symbol *invalid;
symbol *package_marker;
symbol *plus_sign;
symbol *minus_sign;
symbol *dot;
symbol *decimal_point;
symbol *ratio_marker;
//Types
symbol *numtype;
symbol *realtype;
symbol *rattype;//rational
symbol *ttype;
symbol *listtype;
symbol *fixnumtype;
symbol *bignumtype;
symbol *ratiotype;
symbol *singletype;
symbol *base_chartype;
symbol *vectortype;
symbol *arraytype;
symbol *compiled_functiontype;
symbol *stringtype;
symbol *symboltype;
symbol *functiontype;
symbol *hash_tabletype;
symbol *packagetype;
symbol *procinfotype;

/*Common-Lisp symbols*/
//Internal symbols, for me.
symbol *special_operators;
//Defined symbols
symbol *ts;//T symbol
symbol *nils;//NIL symbol
symbol *package_sym;//*package*
symbol *readtable;//*readtable*
symbol *types;//Internal  list of types.
//Lambda list control symbols
symbol *lambda_list_keywords;
symbol *optional;//&optional
symbol *rest;//&rest
symbol *keyword;//&keyword
symbol *aux;//&aux
symbol *whole;//&whole
symbol *body;//&body
symbol *allow_other_keys;//&allow-other-keys
//List function names
symbol *cars;//CAR symbol
symbol *cdrs;
//Special operators
symbol *quote;//QUOTE symbol
//Equality function names
symbol *chareqs;
symbol *charequals;
symbol *stringeqs;
symbol *stringequals;
symbol *eqs;
symbol *eqls;
symbol *equals;
symbol *equalps;
//Reader function names
symbol *read_chars;
symbol *reads;

/*Local functions*/
symbol *initsym(char *name, package *p);
symbol *initintsym(char *name, package *p);
symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env));

void init_keyword_pkg();

void init_cl_pkg();
void init_readtable();
void init_lambda_control();
void init_special_operators();
void init_list_funs();
void init_eq_funs();
void init_read_funs();
void init_set_funs();
void init_types();

cons *initread(stream *str, cons *env);


/*Initialization helper functions*/
symbol *initsym(char *name, package *p)
{
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  if (p == keyword_pkg)
    {
      a->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else if (p == cl_pkg)
    {
      a->plist = fcons(fcons((cons*)external, t), nil);
    }
  return a;
}

symbol *initintsym(char *name, package *p)
{//Init internal symbol
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  if (p == keyword_pkg)
    {
      a->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else if (p == cl_pkg)
    {
      a->plist = fcons(fcons((cons*)internal, t), nil);
    }
  return a;
}

symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env))
{
  symbol *funsym;
  compiled_function *f;

  funsym = intern(strtolstr(name), p);
  funsym->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  funsym->fun = (function*)newcompiled_function();
  f = (compiled_function*)funsym->fun;
  f->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  f->env = basic_env;
  f->lambda_list = lambda_list;
  f->fun = fun;
  return funsym;
}

cons *initread(stream *str, cons *env)
{//Inflexible read. A big kludge. The only code it ever reads is hard-coded bootstrap. 
  package *p = (package*)((procinfo*)env->car)->package_sym->value;
  base_char *c = read_char(str);

  while (1)
    {
      if ((c->c >= 'a') 
	  && (c->c <='z'))
	c->c = c->c - 'A';//convert to uppercase.
      if (c->c == 0 || c->c == ')')
	{
	  if (c->c == 0)
	    unread_char(c, str);
	  return nil;
	}
      else if (c->c == ' ')
	c = read_char(str);
      else if (c->c == '(')
	{//Read a list
	  cons *foo = newcons();
	  cons *bar;
	  cons *to_ret = foo;

	  bar = (cons*)initread(str, env);
	  if (bar == nil)
	    return nil;
	  else
	    foo->car = bar;
	  
	  while ((c->c != ')') &&
		 (c->c != 0))
	    {
	      bar = (cons*)initread(str, env);
	      if (bar == nil)
		return to_ret;
	      foo->cdr = newcons();
	      foo = foo->cdr;
	      foo->car = bar;
	    }
	}
      else if (c->c >='0' && c->c <='9')
	return 0;//TODO parse number.
      else if (c->c == '#')
	{//Read macro character.
	  c = read_char(str);
	  if (c->c == '\\')
	    {//read character
	      c = read_char(str);
	      return (cons*)ctolc(c->c);
	    }
	  else
	    c = read_char(str);
	}
      else
	{
	  int i=0;
	  char name[100];
	  cons *foo=0;

	  while ((c->c != '(') &&
		 (c->c != ')') &&
		 (c->c != ' ') &&
		 (c->c != 0))
	    {
	      if (i >= 100)
		return 0;//error
	      name[i] = c->c;
	      c = read_char(str);
	      i++;
	    }
	  if (i >= 100)
	    return 0;//error
	  else
	    name[i] = 0;
	  
	  if (c->c == 0)
	    unread_char(c, str);
	  
	  foo = (cons*)intern(strtolstr(name), p);
	  return foo;
	}
    }
}

/*Initialization*/
void init_keyword_pkg()
{
  //Init keyword package
  array *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  //External and constant must be initialized manually, because they depend on themselves.
  external = intern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = intern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  constant->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));

  //Other symbol attributes
  internal = initsym("INTERNAL", keyword_pkg);
  inherited = initsym("INHERITED", keyword_pkg);
  dynamic = initsym("DYNAMIC", keyword_pkg);

  //Readtable character attributes
  constituent = initsym("CONSTITUENT", keyword_pkg);
  whitespace = initsym("WHITESPACE", keyword_pkg);
  terminating_macro = initsym("TERMINATING-MACRO-CHARACTER", keyword_pkg);
  non_terminating_macro = initsym("NON-TERMINATING-MACRO-CHARACTER", keyword_pkg);
  single_escape = initsym("SINGLE-ESCAPE", keyword_pkg);
  multiple_escape = initsym("MULTIPLE-ESCAPE", keyword_pkg);

  //Consituent traits
  invalid = initsym("INVALID", keyword_pkg);
  alphabetic = initsym("ALPHABETIC", keyword_pkg);
  alphadigit = initsym("ALPHADIGIT", keyword_pkg);
  package_marker = initsym("PACKAGE-MARKER", keyword_pkg);

  //Types
  ttype = initsym("T", keyword_pkg);
  listtype = initsym("LIST", keyword_pkg);
  fixnumtype = initsym("FIXNUM", keyword_pkg);
  bignumtype = initsym("BIGNUM", keyword_pkg);
  ratiotype = initsym("RATIO", keyword_pkg);
  singletype = initsym("SINGLE", keyword_pkg);
  base_chartype = initsym("BASE-CHARACTER", keyword_pkg);
  vectortype = initsym("ARRAY", keyword_pkg);
  arraytype = initsym("ARRAY", keyword_pkg);
  compiled_functiontype = initsym("COMPILED-FUNCTION", keyword_pkg);
  stringtype = initsym("STRING", keyword_pkg);
  symboltype = initsym("SYMBOL", keyword_pkg);
  functiontype = initsym("FUNCTION", keyword_pkg);
  hash_tabletype = initsym("HASH-TABLE", keyword_pkg);
  packagetype = initsym("PACKAGE", keyword_pkg);
  procinfotype = initsym("PROCESS-INFO-TABLE", keyword_pkg);
}

void init_cl_pkg()
{
  //Init cl package
  array *cl_name = strtolstr("COMMON-LISP");
  cl_pkg = newpackage();
  cl_pkg->name = cl_name;

  //Init t
  t->type = T;
  ts = initsym("T", cl_pkg);
  ts->value = t;
  
  //Init nil
  //array *nil_name = strtolstr("NIL");
  nil->type = LIST;
  nil->car = nil;
  nil->cdr = nil;
  nils = initsym("NIL", cl_pkg);

  nils->value = nil;

  package_sym = initsym("*PACKAGE*", cl_pkg);
  package_sym->value = (cons*)cl_pkg;
  proc->package_sym  = package_sym;
  init_readtable();
  init_lambda_control();
  init_special_operators();
  init_list_funs();
  init_eq_funs();
  init_read_funs();
  init_set_funs();
  init_types();
}

void init_readtable()
{
  long i;

  //Init *readtable*
  array *readtable_name = strtolstr("*READTABLE*");
  readtable = intern(readtable_name, cl_pkg);
  readtable->value = (cons*)newsimple_vector(255);
  char alphabetic_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  char alphadigit_chars[] = "0123456789";
  char whitespace_chars[] = " \t\n";
  char terminating_macro_chars[] = "\"\'(),;`";
  char non_terminating_macro_chars[] = "#";
  char single_escape_chars[] = "\\";
  char multiple_escape_chars[] = "|";
  char constituents[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ1234567890!$_-./:;?+<=>#%&*@[]{}^~";
  long readtable_length = ((array*)readtable->value)->length->num;

  char *c;
  base_char *bc;

  //Create the default readtable
  //Use the numerical ASCII values of each number as the index of a readtable, a simple vector.
  //Each entry has an associated property list, as per CLHS 2.1.4.

  for (i=0;i<' ';i++)
    ((array*)readtable->value)->a[0][i] = fcons(fcons((cons*)invalid, t), ((array*)readtable->value)->a[0][i]);

  for (c=constituents;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] = fcons(fcons((cons*)constituent, t), ((array*)readtable->value)->a[0][*c]);
  //init constituents
  ((array*)readtable->value)->a[0][':'] =  fcons(fcons((cons*)package_marker, t), ((array*)readtable->value)->a[0][':']);

  for (c=alphabetic_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] = fcons(fcons((cons*)alphabetic, t), ((array*)readtable->value)->a[0][*c]);	       

  for (c=alphadigit_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] = fcons(fcons((cons*)alphadigit, t), ((array*)readtable->value)->a[0][*c]);

  for (c=terminating_macro_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] =  fcons(fcons((cons*)terminating_macro, nil), ((array*)readtable->value)->a[0][*c]);

  for (c=whitespace_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] = fcons(fcons((cons*)whitespace, t), fcons((cons*)invalid, ((array*)readtable->value)->a[0][*c]));

  for (c=single_escape_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] =  fcons(fcons((cons*)single_escape, t), ((array*)readtable->value)->a[0][*c]); 

  for (c=multiple_escape_chars;*c!=0;c++)
    ((array*)readtable->value)->a[0][*c] =  fcons(fcons((cons*)multiple_escape, t), ((array*)readtable->value)->a[0][*c]);

  for(i=0;i<readtable_length;i++)
    if (((array*)readtable->value)->a[0][i] == nil)
      ((array*)readtable->value)->a[0][*c] =  fcons(fcons((cons*)invalid, t), ((array*)readtable->value)->a[0][*c]);
  //Invalidate the rest, the wretched lot...  
}

void init_lambda_control()
{
  //Init lambda list control symbols
  optional = initsym("&OPTIONAL", cl_pkg);
  rest = initsym("&REST", cl_pkg);
  keyword = initsym("&KEYWORD", cl_pkg);
  aux = initsym("&AUX", cl_pkg);
  whole = initsym("&WHOLE", cl_pkg);
  body = initsym("&BODY", cl_pkg);
  allow_other_keys = initsym("&ALLOW-OTHER-KEYS", cl_pkg);

  lambda_list_keywords = initsym("LAMBDA-LIST-KEYWORDS", cl_pkg);
  lambda_list_keywords->value = fcons((cons*)optional,
				      fcons((cons*)rest,
					    fcons((cons*)keyword,
						  fcons((cons*)aux,
							fcons((cons*)whole,
							      fcons((cons*)body,
								    fcons((cons*)allow_other_keys, nil)))))));
}

void init_special_operators()
{
  special_operators = initintsym("SPECIAL-OPERATORS", cl_pkg);
  special_operators->value = nil;
}

void init_list_funs()
{
  //symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env));
  cars = initcfun("CAR", 
		  fcons((cons*)intern(strtolstr("LIST"), cl_pkg),
			nil),
		  cl_pkg,
		  &lcar);
  cdrs = initcfun("CDR", 
		  fcons((cons*)intern(strtolstr("LIST"), cl_pkg),
			nil),
		  cl_pkg,
		  &lcar);
  quote = initcfun("QUOTE", 
		   fcons((cons*)intern(strtolstr("EXP"), cl_pkg),
			 nil),
		   cl_pkg,
		   &lquote);  
}  

void init_eq_funs()
{
  chareqs = initcfun("CHAR=",
		     fcons((cons*)intern(strtolstr("A"), cl_pkg),
			   fcons((cons*)intern(strtolstr("B"), cl_pkg),
				 nil)),
		     cl_pkg,
		     &lchareq);
  charequals = initcfun("CHAR-EQUAL",
			fcons((cons*)intern(strtolstr("A"), cl_pkg),
			      fcons((cons*)intern(strtolstr("B"), cl_pkg),
				    nil)),
			cl_pkg,
			&lcharequal);

  stringeqs = initcfun("STRING=",
		       fcons((cons*)intern(strtolstr("A"), cl_pkg),
			     fcons((cons*)intern(strtolstr("B"), cl_pkg),
				   nil)),
		       cl_pkg,
		       &lstringeq);
  
  stringequals = initcfun("STRING-EQUAL",
			  fcons((cons*)intern(strtolstr("A"), cl_pkg),
				fcons((cons*)intern(strtolstr("B"), cl_pkg),
				      nil)),
			  cl_pkg,
			  &lstringequal);
  
  eqs = initcfun("EQ", 
		 fcons((cons*)intern(strtolstr("A"), cl_pkg), 
		       fcons((cons*)intern(strtolstr("B"), cl_pkg), 
			     nil)), 
		 cl_pkg, 
		 &leq);

  eqls = initcfun("EQL",
		  fcons((cons*)intern(strtolstr("A"), cl_pkg),
			fcons((cons*)intern(strtolstr("B"), cl_pkg),
			      nil)),
		  cl_pkg,
		  &leql);
}

void init_read_funs()
{
  stream *str = newstream();
  str->rv = strtolstr("(&OPTIONAL (STREAM *STANDARD-INPUT*) (EOF-ERROR-P T))");
  str->write_index = str->rv->length->num;

  read_chars = initcfun("READ-CHAR",
			initread(str, basic_env),
			cl_pkg,
			&lread_char);
}  

void init_set_funs()
{
  eqls = initcfun("DEFUN",
		  fcons((cons*)intern(strtolstr("SYMBOL"), cl_pkg),
			fcons((cons*)intern(strtolstr("LAMBDA-LIST"), cl_pkg),
			      fcons((cons*)intern(strtolstr("FORM"), cl_pkg),
				    nil))),
		  cl_pkg,
		  &leql);
}

void init_types()
{
  array *tv;//Tpyes vector
  
  types = initintsym("TYPES", cl_pkg);
  types->value = (cons*)newsimple_vector(1024);
  tv=(array*)types->value;
  //Initialize type hierarchies

}

procinfo *init()
{
  long i;
  long j;

  basic_env = extend_env(nil);

  //init process info
  proc = malloc(sizeof(procinfo));
  proc->type = PROCINFO;
  proc->packages = newcons();
  proc->packages->car = (cons*)cl_pkg;
  proc->packages->cdr = newcons();
  proc->packages->cdr->car = (cons*)cl_user_pkg;
  proc->packages->cdr->cdr = newcons();
  proc->packages->cdr->cdr->car = (cons*)keyword_pkg;  
  basic_env->car = (cons*)proc;

  init_keyword_pkg();
  init_cl_pkg();
  
  //Init cl-user package
  array *cl_user_name = strtolstr("COMMON_LISP_USER");
  cl_user_pkg = newpackage();
  cl_user_pkg->name = cl_user_name;


  return proc;
}

