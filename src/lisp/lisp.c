/* lisp.c
 *
 * Daniel Ralston, 2009
 *
 * The actual code which makes Lisp tick. Functions here are targeted for use
 * by the C code. To be used in a Lisp environment, add a short interface 
 * function in lbind.c. You will notice a complete lack of de-allocation in
 * this code. If this is a concern, what's the matter with you? This is Lisp!
 * It's garbage collected! At least, it will be.
 *
 * This code is released under the GNU GPL General Public License.
 */

//>TODO add special operators
//>TODO add read function
//TODO make sure nil can't get clobbered
//TODO cut out automatic export of symbols in init.c, and make symbol export
//    a separate function.
//TODO 

#include "lisp.h"
#include <stdlib.h>
#include "init.h"

#include <stdio.h>


/* Internal Function Declarations */
cons *subtypep(cons *object, cons *objtype, symbol *type);
cons *evalambda_base(cons *lambda_list, cons *args, cons *env, cons *evaluate);

cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;


cons *type_of(cons *foo)
{
  if (foo->type <= (cons*)BUILT_IN_CLASS)
    /* This line returns a warning. No way to suppress it, I'm afraid. :( It 
     * doesn't matter if the two types happen to be of different sizes,
     * because these are just our constant type specifiers, which are quite
     * compact.
     */
    return basic_classes[(int)foo->type];
  else
    return foo->type;
}

cons *subtypep(cons *object, cons *objtype, symbol *type)
{
  cons *supertype_list = objtype->cdr->car;
  symbol *supertype;
  if (type->class == objtype)
    return t;
  for (supertype_list;supertype_list!=nil;supertype_list=supertype_list->cdr)
    {
      supertype = (symbol*)supertype_list->car;
      if (subtypep(object, supertype->class, type) == t)
	return t;
      else
	continue;
    }
  return nil;
}

cons *typep(cons *object, symbol *type)
{
  cons *objtype = type_of(object);
  return subtypep(object, objtype, type);
}

/* Object allocation routines */
cons *newcons()
{
  cons *c = malloc(sizeof(cons));
  c->type = (cons*)LIST;
  c->car = nil;
  c->cdr = nil;
  return c;
}

fixnum *newfixnum(long num)
{
  fixnum *f = malloc(sizeof(fixnum));
  f->type = (cons*)FIXNUM;
  f->num = num;
  return f;
}

bignum *newbignum()
{
  bignum *b = malloc(sizeof(bignum));
  b->type = (cons*)BIGNUM;
  b->num = 0;
  b->next = (bignum*)nil;
  return b;
}

ratio *newratio(fixnum *n, fixnum *d)
{
  ratio *r = malloc(sizeof(ratio));
  r->type = (cons*)RATIO;
  r->numerator = n;
  r->denominator = d;
  return r;
}

single *newsingle()
{
  single *s = malloc(sizeof(single));
  s->type = (cons*)SINGLE;
  s->sign = 0;
  s->base = 0;
  s->exponent = 0;
  s->integer = 0;
  return s;
}

base_char *newbase_char()
{
  base_char *c = malloc(sizeof(base_char));
  c->type = (cons*)BASE_CHAR;
  c->c = 0;
  return c;
}

array *newarray(long rank, int length)
{
  long i;
  array *a = malloc(sizeof(array));
  a->type = (cons*)ARRAY;
  a->plist = nil;
  a->rank = newfixnum(rank);
  a->length = newfixnum(length);
  a->a = malloc(rank * sizeof(cons*));
  for(i=0;i<rank;i++)
    a->a[i] = malloc(length * sizeof(cons*));
  for (rank--;rank>=0;rank--)
    {
      for (i=0;i<length;i++)
	a->a[rank][i] = nil;
    }
  a->next = (array*)0;
  return a;
}

array *newsimple_vector(int length)
{
  array *v = newarray(1, length);
  v->type = (cons*)VECTOR;
  return v;
}

hash_table *newhash_table(int length, function *eq_fun)
{
  hash_table *ht = malloc(sizeof(hash_table));
  ht->type = (cons*)HASH_TABLE;
  ht->plist = nil;
  ht->a = newsimple_vector(length);
  ht->eq_fun = eq_fun;
  return ht;
}

package *newpackage()
{
  package *p = malloc(sizeof(package));
  p->type = (cons*)PACKAGE;
  p->plist = nil;
  p->name = (array*)nil;
  p->global = newsimple_vector(HASH_TABLE_SIZE);
  return p;
}

compiled_function *newcompiled_function()
{
  compiled_function *f = malloc(sizeof(compiled_function));
  f->type = (cons*)COMPILED_FUNCTION;
  f->plist = nil;
  f->lambda_list = nil;
  f->env = 0;
  f->fun = 0;
}

function *newfunction()
{
  function *f = malloc(sizeof(function));
  f->type = (cons*)FUNCTION;
  f->plist = nil;
  f->lambda_list = nil;
  f->env = 0;
  f->fun = 0;
}

stream *newstream()
{
  stream *str = malloc(sizeof(stream));
  str->type = (cons*)STREAM;
  str->plist = nil;
  str->read_index = 0;
  //Manual setup of the new stream will be required.
  str->rv = (array*)0;
  str->wv = (array*)0;
  str->write_index = 0;

  return str;
}

//Helper functions to translate C stuff into Lisp objects

/* Convert a C character to a Lisp base character.
 */
base_char *ctolc(char c)
{//Char to Lisp Charx
  base_char *fred = newbase_char();
  fred->c = c;
  return fred;
}

/* Convert a C string (C array of C characters) to a Lisp
 * string.
 */
array *strtolstr(char *str)
{//C string to Lisp string.
  int string_len;
  long i;
  base_char *c = 0;
  array *to_ret;

  for(string_len=1;*(str+string_len-1)!=0;string_len++);
  //Find the string length.
  to_ret = newsimple_vector(string_len);

  to_ret->type = (cons*)STRING;
 
  for(i=0;*str!=0;i++)
    {//Add the base_chars to the string.
      c = newbase_char();
      to_ret->a[0][i] = (cons*)c;
      c->c = *str;
      str++;
    }
  c = newbase_char();
  to_ret->a[0][i] = (cons*)c;
  c->c = 0;
  return to_ret;
}

//Primitives

/* Decide whether or not an object is nil. An object is nil if it is the nil
 * object, or an empty list.
 */
cons *null (cons *a)
{
  if (a == nil ||
      ((typep(a, list_s) == t) &&
       (a->car == nil) &&
       (a->cdr == nil)))
    return t;
  else
    return nil;
}

/* Decide whether or not an object is a number.
 */
cons *numberp(cons *a)
{
  return typep(a, number_s);
}

/* CONS two objects together. Prefixed with 'f' to prevent name collision with 
 * the structure.
 */
cons *fcons(cons *a, cons *b)
{
  cons *to_ret = malloc(sizeof(cons));
  to_ret->type = (cons*)LIST;  
  to_ret->car = a;
  to_ret->cdr = b;
  return to_ret;
}

/* Return the CAR of a CONS cell.
 */
cons *car(cons *a)
{
  if (typep(a, list_s) == t)
    return a->car;
  else
    return 0;//TODO error
}

/* Replace the CAR of a CONS cell.
 */
cons *rplaca(cons *a, cons *new)
{
  if ((typep(a, list_s) == t) && 
      (a != nil))
    {
      a->car = new;
      return a;
    }
  else
    return 0;//TODO error
}

/* Return the CDR of a CONS cell.
 */
cons *cdr(cons *a)
{
  if (typep(a, list_s) == t)
    return a->cdr;
  else
    return 0;//TODO error
}

/* Replace the CDR of a CONS cell.
 */
cons *rplacd(cons *a, cons *new)
{
  if ((typep(a, list_s) == t) && 
      (a != nil))
    {
      a->cdr = new;
      return a;
    }
  else
    return 0;//TODO error
}

/* Intern a symbol into a package, as per CLHS: "INTERN". Return the symbol of 
 * name in package p. If it does not exist, create it.
 */
symbol *intern(array *name, package *p)
{
  long i;
  unsigned long index ;
  char hashed_name[4];
  cons *entry;
  symbol *s;

  for (i=0;i<3;i++)
    {/* Hash the first four characters of the name.
      */
      if (i >= name->length->num)
	hashed_name[i] = 0;
      else
	hashed_name[i] = ((base_char*)name->a[0][i])->c;
    }
  index = *(unsigned int*)&hashed_name[0] % HASH_TABLE_SIZE;
  entry = p->global->a[0][index];

  if (entry != nil)
    {
      while(entry != nil)
	{/* Try to find a a symbol of the same name already interned into p.
	  */
	  s = (symbol*)entry->car;
	  if (stringequal(name, s->name) == t)
	    return s;
	  else if (entry->cdr == nil)
	    {/* If we've run out of entries, add a new one
	      */
	      entry->cdr = newcons();
	      entry = entry->cdr;
	      entry->car = (cons*)malloc(sizeof(symbol));
	      break;
	    }
	  else
	    entry = entry->cdr;
	}
    }
  else if (entry == nil)
    {/* Create a new entry for the hash value in the table
      */
      p->global->a[0][index] = newcons();
      p->global->a[0][index]->car = (cons*)malloc(sizeof(symbol));
      entry = p->global->a[0][index];
    }
  s = (symbol*)entry->car;
  s->plist = nil;
  s->type = (cons*)SYMBOL;
  s->name = name;
  s->home_package = p;
  if (p == keyword_pkg)
    {
      s->value = (cons*)s;
      s->plist = setassoc((cons*)external, t, s->plist);
    }
  else
    s->value = 0;
  s->fun = (function*)0;
  return s;
}

/* Determine if two characters are equal, as per CLHS: "CHAR=".
 */
cons *chareq(base_char *a, base_char *b)
{
  if (a==b)
    return t;
  else if (a->type != b->type)
    return 0;//TODO error?
  else if ((!(a->type <= (cons*)BUILT_IN_CLASS) &&
	    !(a->type == (cons*)BASE_CHAR)))
    return 0;//TODO error!
  else if (a->c == b->c)
    return t;
  else
    return nil;
}

/* Determine if two characters are equal, as per CLHS: "CHAR-EQUAL"
 */
cons *charequal(base_char *a, base_char *b)
{
  char ac;
  char bc;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return 0;//TODO error?
  else if ((!(a->type <= (cons*)BUILT_IN_CLASS) &&
	    !(a->type == (cons*)BASE_CHAR)))
    return 0;//TODO error!

  ac = a->c;
  if (ac<='z' && ac>='a')
    ac = ac-'a';

  bc = b->c;
  if (bc<='z' && bc>='a')
    bc = bc-'a';

  else if (ac == bc)
    return t;
  else
    return nil;
}

/* Determine if two strings are equal, as per CLHS: "STRING="
 */
cons *stringeq(array *a, array *b)
{
  long i=0;
  if(a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  else if (((a->type <= (cons*)BUILT_IN_CLASS) &&
	    (a->type == (cons*)STRING)))
    return 0;//TODO error
  
  while(1)
    {
      if (chareq((base_char*)a->a[0][i], (base_char*)b->a[0][i]) == nil)
	return nil;
      else if (((base_char*)a->a[0][i])->c == 0) 
	return t;
      else
	i++;
    }
}

/* Determine if two strings are equal, as per CLHS: "STRING-EQUAL"
 */
cons *stringequal(array *a, array *b)
{
  long i=0;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  else if (((a->type <= (cons*)BUILT_IN_CLASS) &&
	    (a->type == (cons*)STRING)))
    {
      while(1)
	{
	  if (charequal((base_char*)a->a[0][i], (base_char*)b->a[0][i]) == nil)
	    return nil;
	  else if (((base_char*)a->a[0][i])->c == 0) 
	    return t;
	  else
	    i++;
	}
    }
  else
    return 0;//TODO error
}

/* Find a package by name, from a process' procinfo.
 */
package *find_package(array *name, procinfo *pinfo)
{
  cons *p = pinfo->packages;
  for (p;p!=nil;p=p->cdr)
    {
      if (stringequal(((package*)p->car)->name, name) == t)
	return (package*)p->car;
    }
  return (package*)nil;
}

/* Determine if two objects are equal, as per CLHS: "EQ"
 */
cons *eq (cons *a, cons *b)
{
  if (a==b)
    return t;
  else
    return nil;
}

/* Determine if two objects are equal, as per CLHS: "EQL"
 * ->Incomplete
 */
cons *eql (cons *a, cons *b)
{
  if (a == b)
    return t;
  else if (a->type == b->type)
    {
      symbol *typename = (symbol*)type_of(a)->car;
      if (typep(a, number_s) == t)
	{
	  if (typename == fixnum_s)
	    {  
	      if (((fixnum*)a)->num == ((fixnum*)b)->num)
		return t;
	    }
	  else if (typename == bignum_s)
	    {
	      while (((bignum*)a)->num == ((bignum*)b)->num)
		{
		  if (((cons*)((bignum*)a)->next == nil) && 
		      ((cons*)((bignum*)b)->next == nil))
		    return t;
		  a = (cons*)((bignum*)a)->next;
		  b = (cons*)((bignum*)b)->next;	
		}
	      return nil;
	    }
	  else if (typename == ratio_s)
	    {
	      if ((((ratio*)a)->numerator == ((ratio*)b)->numerator) &&
		  (((ratio*)a)->denominator == ((ratio*)b)->denominator))
		return t;
	      else
		return nil;
	    }
	  else if (typename == single_s)
	    {
	      if ((((single*)a)->sign == ((single*)b)->sign) &&
		  (((single*)a)->base == ((single*)b)->base) &&
		  (eql((cons*)((single*)a)->exponent, 
		       (cons*)((single*)b)->exponent) == t) &&
		  (eql((cons*)((single*)a)->integer, 
		       (cons*)((single*)b)->integer)))
		return t;
	      else
		return nil;
	    }
	}
      else if (typep(a, character_s) == t)
	return chareq((base_char*)a, (base_char*)b);
      else
	return nil;
    }
}

int hash(cons *object, hash_table *ht)
{
  int objsize;
  int index = 0;
  int ht_length = ht->a->length->num;
  char *i = (char*)object;
  
  for (objsize;objsize>0;i--)
    //TODO optimize for different types.
    index += *i;
  index = index % ht_length;

  return index;
}


/* Lookup a symbol in the current environment; can't do foreign packages. 
 */
cons *lookup(char *namestr, cons *env)
{
  array *name = strtolstr(namestr);
  symbol *s = intern(name, 
		     (package*)
		     ((symbol*)((procinfo*)env->car)->package_s)->value);
  return eval((cons*)s, env);
}

/* Create an association pair.
 * Equivalent to fcons(key, fcons(value, nil))
 */
cons *mkpair(cons *key, cons *value)
{
  cons *a = newcons();
  a->car = key;
  a->cdr = newcons();
  a->cdr->car = value;
  return a;
}

/* Evaluate a Lisp expression.
 * Note that it does not by nature return a dynamic binding if a variable is
 * declared special. The binding forms (let, let*, etc) will create dynamic 
 * bindings instead of lexical ones. Function calls will still make lexical
 * parameter bindings. This way, eval can check lexical bindings, and have
 * lexical and dynamic variables peacefully coexist properly.
 */
cons *eval(cons *exp, cons *env)
{
  if (exp == nil)
    return nil;
  else if (exp == t)
    return t;
  else if (typep(exp, symbol_s) == t)
    {/* If the expression is just a symbol, evaluate it, and return its current
      * value.
      */

      symbol *s = (symbol*)exp;
      cons *binding = env->cdr; /* current environment node */
      
      while (binding!=nil)
	{/* Loop through the lexical environment, searching for a binding for 
	  * symbol. 
	  */
	  if ((symbol*)binding->car->car == s)
	    /* If the current binding's symbol is the one we're looking for, 
	     * return the value of the binding.
	     */
	    return binding->car->cdr->car;
	  else if (binding->cdr == nil)
	    /* If we've run out of bindings at this "level" of the environment, 
	     * move to the next one.
	     */
	    binding = binding->car->cdr;
	  else
	    /* If there are still bindings to search, go to the next one.
	     */
	    binding = binding->cdr;
	}
      if (s->value != 0)
	/* If execution reaches this spot, the lexical search has been 
	 * unfruitful. Return the dynamic binding, unless there is none, in
	 * which case, raise an error. TBD via CLHS
	 */
	return s->value;
      else
	return 0;//TODO no value error
    }
  else if ((typep(exp, list_s) == t) && 
	   (typep(exp->car, list_s) == nil))
    {/* If the expression is a list, whose first object is not a list, evaluate 
      * it as a form.
      */
      symbol *s = (symbol*)exp->car;
      function *f;
      if (typep(exp->car, symbol_s) == t)
	{/* If the first object is a symbol, treat is as a function or macro 
	  * name.
	  */
	  f = (function*)s->fun;
	  if (f == (function*)0)
	    /* If there is no function bound, raise an error. TBD via CLHS
	     */
	    return 0;
	}
      else if ((typep(exp->car, function_s) == t) ||
	       (typep(exp->car, compiled_function_s) == t))
	/* If the first object IS a function, treat it as such! Used primarily
	 * for lambda.
	 */
	f = (function*)exp->car;
            
      if (type_of((cons*)f) == function_s->class)
	{/* If the function isn't compiled, bind the arguments to the function's
	  * variable names, and evaluate the function's form.
	  */
	  cons *newenv = extend_env(f->env);
	  if (assoc((cons*)special_operator, f->plist)->cdr != nil)
	    /* If the expression is a special form, evaluate it as a special 
	     * form, and treat its paramteres accordingly.
	     */
	    newenv = evalambda_special(f->lambda_list, exp->cdr, newenv);
	  else
	    newenv = evalambda(f->lambda_list, exp->cdr, newenv);
	  return eval(f->fun, evalambda(f->lambda_list, exp->cdr, newenv));
	}
      else if (type_of((cons*)f) == compiled_function_s->class)
	{/* If the function is compiled, do exactly the same. Except, call the 
	  * function pointer, with the expanded environment as the paramater.
	  */
	  compiled_function *cf = (compiled_function*)f;
	  cons *newenv = extend_env(cf->env);
	  if (assoc((cons*)special_operator, cf->plist)->cdr != nil)
	    /* If the expression is a special form, evaluate it as a special 
	     * form, and treat its paramteres accordingly.
	     */
	    newenv = evalambda_special(f->lambda_list, exp->cdr, newenv);
	  else
	    newenv = evalambda(f->lambda_list, exp->cdr, newenv);
	  return cf->fun(newenv);
	}
      else
	/* If the first object of the list is anything else, throw an error, for
	 * it is invalid, and cannot be executed.
	 */
	return 0;//TODO error, not a function
    }
  else if (typep(exp, list_s) == nil)
    /* An other valid input must be self-evaluating.
     */
    return exp;

  else
    /* Because I am the mighty programmer, anything I have not anticipated must 
     * be wrong. Throw an error, because it's not a valid expression.
     */
    return 0;//TODO should be error
}

/* Creates a new lexical environment level. New lexical bindings will be pushed 
 * in before the previous one. When the evaluator, looking for bindings, reaches
 * a lexical binding whose cdr is nil, it will know to descend to the next 
 * level. When both the car and cdr of the last CONS cell are nil, there are no 
 * more lexical levels. The car of any environment will be the  procinfo.
 */
cons *extend_env(cons *env)
{
  cons *oldenv = env;
  cons *newenv = newcons();
  newenv->car = oldenv->car;//procinfo
  newenv->cdr = newcons();
  newenv->cdr->car = oldenv;
  //return fcons(env->car, fcons(env->cdr, nil);
  return newenv;
}

cons *envbind(symbol *sym, cons *value, cons *env)
{/* Bind a new value to a symbol in a lexical environment.
  */
  cons *first = env;  
  cons *newenv = newcons();
  newenv->car = fcons((cons*)sym, fcons(value, nil));
  newenv->cdr = env->cdr;
  env->cdr = newenv;
  return env;
}

cons *evalambda(cons *lambda_list, cons *args, cons *env)
{
  return evalambda_base(lambda_list, args, env, t);
}

cons *evalambda_special(cons *lambda_list, cons *args, cons *env)
{
  return evalambda_base(lambda_list, args, env, nil);
}

cons *evalambda_base(cons *lambda_list, cons *args, cons *env, cons *evaluate)
{/* Lexically bind a set of arguments to variable names defined in a lambda 
  * list.
  */
  //NOTE: "Someone, please tidy me." :'( "I'm so messy!"
  cons *oldenv = env;
  cons *binding;
  cons *foo;
  array *varname;
  symbol *varsym;
  
  while((null(lambda_list) == nil) && 
	(null(args) == nil) &&
	(lambda_list->car != (cons*)optional_s) &&//Break on 
	(lambda_list->car != (cons*)rest_s) &&
	(lambda_list->car != (cons*)keyword_s) &&
	(lambda_list->car != (cons*)aux_s))
    {/* Bind compulsory parameters to the proper variable names.
      */
      varsym = (symbol*)lambda_list->car;
      if (evaluate == t)
	envbind(varsym, eval(args->car, env), env);
      else
	envbind(varsym, args->car, env);
      
      lambda_list = lambda_list->cdr;
      args = args->cdr;
    }

  if (lambda_list->car == (cons*)optional_s)
    {/* Bind &optional parameters to proper variable names.
      */
      lambda_list = lambda_list->cdr;
      while((null(lambda_list) == nil) && 
	    (null(args) == nil) &&
	    (lambda_list->car != (cons*)rest_s) &&
	    (lambda_list->car != (cons*)keyword_s) &&
	    (lambda_list->car != (cons*)aux_s))
	{/* If the parameter list hasn't run out, bind them to variable names, 
	  * as per the lambda list.
	  */
	  if (typep(lambda_list->car, symbol_s) == t)
	    varsym = (symbol*)lambda_list->car;
	  else if ((typep(lambda_list->car, list_s) == t) &&
		   (typep(lambda_list->car->car, symbol_s) == t))
	    varsym = (symbol*)lambda_list->car->car;
	  else
	    return 0;//TODO error

	  if (evaluate == t)
	    envbind(varsym, eval(args->car, env), env);
	  else
	    envbind(varsym, args->car, env);

	  lambda_list = lambda_list->cdr;
	  args = args->cdr;
	}
      while((null(lambda_list) == nil) && 
	    (null(args) == t) &&
	    (lambda_list->car != (cons*)rest_s) &&
	    (lambda_list->car != (cons*)keyword_s) &&
	    (lambda_list->car != (cons*)aux_s))
	{/* If the end of the parameter list has been reached, associate the 
	  * variable names with their default values, if any. If there is no 
	  * default value for a variable name, bind it to nil.
	  */
	  if (typep(lambda_list->car, symbol_s) == t)
	    {
	      varsym = (symbol*)lambda_list->car;
	      envbind(varsym, nil, env);
	      lambda_list = lambda_list->cdr;
	      args = args->cdr;
	    }
	  else if ((typep(lambda_list->car, list_s) == t) &&
		   (typep(lambda_list->car->car, symbol_s) == t))
	    {
	      varsym = (symbol*)lambda_list->car->car;
	      if (evaluate == t)
		envbind(varsym, eval(lambda_list->car->cdr, env), env);
	      else
		envbind(varsym, lambda_list->car->cdr, env);
	      lambda_list = lambda_list->cdr;
	      args = args->cdr;
	    }
	  else
	    return 0;//TODO error
	}
    }
  if (lambda_list->car == (cons*)rest_s)
    {/* Bind the remainder of the lambda list to the &rest argument, if any.
      */
      lambda_list = lambda_list->cdr;
  
      if((null(lambda_list) == nil) && 
	 (null(args) == nil) &&
	 (lambda_list->car != (cons*)optional_s) &&
	 (lambda_list->car != (cons*)rest_s) && 
	 (lambda_list->car != (cons*)keyword_s) &&
	 (lambda_list->car != (cons*)aux_s))
	{
	  if (typep(lambda_list->car, symbol_s) == t)
	    varsym = (symbol*)lambda_list->car;
	  else
	    return 0;//TODO error

	  envbind(varsym, args, env);
	  lambda_list = lambda_list->cdr;
	  args = args->cdr;
	}
    }
  /* if (lambda_list->car != (cons*)keyword) */
  /*   env = evalkeyword(lambda_list, args, env); */
  /* if (lambda_list->car != (cons*)aux) */
  /*   env = env; */
    return env;
}

cons *assoc(cons *key, cons *plist)
{/* Search for a CONS cell in property list p with property-key key.
  */
  while(plist != nil)
    {
      if (eql(key, plist->car->car) == t)
	return plist->car;
      else
	plist = plist->cdr;
    }
  return nil;
}

cons *setassoc(cons *key, cons *value, cons *plist)
{
  cons *entry = assoc(key, plist);
  if (entry == nil)
    {
      entry = fcons(key, value);
      plist = fcons(entry, plist);
    }
  else
    entry->cdr = value;
  
  return plist;
}

symbol *defun(symbol *sym, cons *lambda_list, cons *form, cons *env)
{/* Define a function in a package.
  */
  function *f;

  sym->fun = newfunction();
  f = (function*)sym->fun;
  f->plist = nil;
  f->env = env;
  f->lambda_list = lambda_list;
  f->fun = form;
  return sym;
}

//Stream functions
base_char *read_char(stream *str)
{/* Read a character from a stream. (See stream implementation documentation in 
  * lisp.h for a better understanding.)
  */
  base_char *to_ret = (base_char*)str->rv->a[0][str->read_index];

  if ((str->read_index == str->write_index) &&
      (str->rv == str->wv))
    /* If the stream has 'dried up', return nil. This is not CLHS okey-dokey, 
     * but a Lisp wrapper function will take care of the extra fidgeting.
     */
    return (base_char*)nil;
  else if (str->read_index < str->rv->length->num)
    /* If the stream has not yet reached the end of the current vector, 
     * increment the index.
     */
      str->read_index++;
  else if (str->rv->next != 0)
    {/* If the stream has reached the end of the vector, and the vector
      * is extended, move to the next vector.
      */
      str->rv = str->rv->next;
      str->read_index = 0;
    }
  else
    /* If the strem has dried up, and there is no more, just increment the 
     * read index.
     */
    str->read_index++;
  return to_ret;
}

base_char *peek_char(stream *str)
{/* Read a character from a stream without officially reading it. hence, peek.
  */
  return (base_char*)str->rv->a[0][str->read_index];
}

cons *unread_char(base_char *c, stream *str)
{/* Place a character at the beginning of a stream, and set the read index to 
  * match, making the character the next character to be read. If the current 
  * vector is at the beginning, make a new, very small, vector, and assign the 
  * old vector to the new one's "next".
  */
  if (str->read_index>0)
    str->read_index--;
  else
    {
      array *old_rv = str->rv;
      str->rv = newsimple_vector(15);
      str->rv->next = old_rv;
      str->read_index = 14;
      str->rv->a[0][14] = (cons*)c;
    }    
  str->rv->a[0][str->read_index] = (cons*)c;
  return nil;
}

cons *write_char(base_char *c, stream *str)
{/* Append a character to a stream.
  */
  if (str->write_index < str->wv->length->num)
    {/* If the stream's write-vector hasn't been exhausted, append the
      * character, and increment the write index.
      */
      str->wv->a[0][str->write_index] = (cons*)c;
      str->write_index++;
    }
  else if (str->wv->next == 0)
    {/* If the stream's write-vector has been exhausted, append a new
      * vector, and add the character.
      */
      str->wv->next = newsimple_vector(128);
      str->wv->a[0][0] = (cons*)c;
      str->write_index = 1;
    }
  return (cons*)c;
}

extern cons *initread(stream *str, cons *env);

int main ()
{
  procinfo *proc = init();
  
  /*Tests*/
  cons *env = extend_env(nil);
  env->car = (cons*)proc;

  cons *exp = fcons((cons*)cons_s, fcons((cons*)external, fcons((cons*)internal, nil)));			  
  
  cons *hope = eval(exp, env);

  stream *str = newstream();
  
  str->rv = strtolstr("(LIST :INTERNAL :EXTERNAL)");
  str->write_index = 19;

  cons *exp2 = (cons*)initread(str, env);
  cons *lesser = eval((cons*)exp2, env);

  cons *xyzzy = eval(fcons((cons*)quote_s, fcons((cons*)quote_s, nil)), env);

  str->read_index = 0;
  str->rv = strtolstr("(LIST :INTERNAL :EXTERNAL)");
  str->write_index = 4;

  cons *snazzy = read(str, env);

  return 0;
}
