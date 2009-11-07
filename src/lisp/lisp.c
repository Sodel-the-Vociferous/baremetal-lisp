#include <stdlib.h>
#include "lisp.h"
#include "init.h"

//I KNOW there are memory leaks EVERYWHERE.
//In fact there isn't a single de-allocation in this code.
//Fret not.
//It will be garbage collected.

//TODO add special operators





cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;

//Object allocation routines
cons *newcons()
{
  cons *c = malloc(sizeof(cons));
  c->type = LIST;
  c->car = nil;
  c->cdr = nil;
  return c;
}

fixnum *newfixnum(long num)
{
  fixnum *f = malloc(sizeof(fixnum));
  f->type = FIXNUM;
  f->num = num;
  return f;
}

bignum *newbignum()
{
  bignum *b = malloc(sizeof(bignum));
  b->type = BIGNUM;
  b->num = 0;
  b->next = (bignum*)nil;
  return b;
}

ratio *newratio(fixnum *n, fixnum *d)
{
  ratio *r = malloc(sizeof(ratio));
  r->type = RATIO;
  r->numerator = n;
  r->denominator = d;
  return r;
}

single *newsingle()
{
  single *s = malloc(sizeof(single));
  s->type = SINGLE;
  s->sign = 0;
  s->base = 0;
  s->exponent = 0;
  s->integer = 0;
  return s;
}

base_char *newbase_char()
{
  base_char *c = malloc(sizeof(base_char));
  c->type = BASE_CHAR;
  c->c = 0;
  return c;
}

array *newarray(long rank, int length)
{
  long i;
  array *a = malloc(sizeof(array));
  a->type = ARRAY;
  a->plist = nil;
  a->rank = newfixnum(rank);
  a->length = newfixnum(length);
  a->a = malloc(rank * sizeof(cons*));
  for(i=0;i<rank;i++)
    a->a[i] = malloc(length * sizeof(cons*));
  //Loops forever here. This is bizarre. FIX.
  
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
  v->type = SIMPLE_VECTOR;
  return v;
}

package *newpackage()
{
  package *p = malloc(sizeof(package));
  p->type = PACKAGE;
  p->plist = nil;
  p->name = (array*)nil;
  p->global = newsimple_vector(HASH_TABLE_SIZE);
  return p;
}

compiled_function *newcompiled_function()
{
  compiled_function *f = malloc(sizeof(compiled_function));
  f->type = COMPILED_FUNCTION;
  f->plist = nil;
  f->lambda_list = nil;
  f->env = 0;
  f->fun = 0;
}

function *newfunction()
{
  function *f = malloc(sizeof(function));
  f->type = FUNCTION;
  f->plist = nil;
  f->lambda_list = nil;
  f->env = 0;
  f->fun = 0;
}

//Helper functions to translate C stuff into Lisp objects

base_char *ctolc(char c)
{//Char to Lisp Charx
  base_char *fred = newbase_char();
  fred->c = c;
  return fred;
}

array *strtolstr(char *str)
{//C string to Lisp string.
  int string_len;
  long i;
  base_char *c = 0;
  array *to_ret;

  for(string_len=1;*(str+string_len-1)!=0;string_len++);
  //Find the string length.
  to_ret = newsimple_vector(string_len);

  to_ret->type = STRING;
 
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
cons *null (cons *a)
{//Is object nil?
  if (a == nil ||
      (a->type = LIST &&
       a->car == nil &&
       a->cdr == nil))
    return t;
  else
    return nil;
}

cons *numberp(cons *a)
{//Is object a number?
  switch (a->type)
    {
    case (FIXNUM):
    case (BIGNUM):
    case (RATIO):
    case (SINGLE):
      return t;
    default:
      return nil;
    }
}

cons *fcons(cons *a, cons *b)
{//f added to prevent collision
  cons *to_ret = malloc(sizeof(cons));
  to_ret->type = LIST;  
  to_ret->car = a;
  to_ret->cdr = b;
  return to_ret;
}

cons *car(cons *a)
{
  if (a->type == LIST)
    return a->car;
  else
    return nil;//TODO error
}

cons *rplaca(cons *a, cons *new)
{
  if (a->type == LIST && a != nil)
    {
      a->car = new;
      return a;
    }
  else
    return nil;//TODO error
}

cons *cdr(cons *a)
{
  if (a->type == LIST)
    return a->cdr;
  else
    return nil;//TODO error
}

cons *rplacd(cons *a, cons *new)
{
  if (a->type == LIST && a != nil)
    {
      a->cdr = new;
      return a;
    }
  else
    return nil;//TODO error
}

/*This is all infrastructure for the intern function. */
symbol *intern(array *name, package *p)
{//HARK. This function doesn't do symbol lookups in other packages with the : and :: syntax. Change this later. :]
  long i;
  unsigned long index ;
  char hashed_name[4];
  cons *entry;
  symbol *s;

  for (i=0;i<3;i++)
    {
      if (i >= name->length->num)
	hashed_name[i] = 0;
      else
	hashed_name[i] = ((base_char*)name->a[0][i])->c;
    }
  index = *(unsigned int*)&hashed_name[0] % HASH_TABLE_SIZE;
  //Hash the first four characters of the name.
  entry = p->global->a[0][index];

  if (entry != nil)
    {
      s = (symbol*)entry->car;

      while(entry != nil)
	{//Try to find a a symbol of the same name already interned into p
	  if (stringequal(name, s->name) == t)
	    return s;
	  else if (entry->cdr == nil)
	    {//If we've run out of entries, add a new one
	      entry->cdr = newcons();
	      entry = entry->cdr;
	      entry->car = (cons*)malloc(sizeof(symbol));
	      break;
	    }
	  else
	    entry = entry->cdr;
	  //next entry
	}
    }
  else if (entry == nil)
    {//Create a new entry for the hash value in the table
      p->global->a[0][index] = newcons();
      p->global->a[0][index]->car = (cons*)malloc(sizeof(symbol));
      entry = p->global->a[0][index];
    }
  s = (symbol*)entry->car;
  s->plist = nil;
  s->type = SYMBOL;
  s->name = name;
  s->home_package = p;
  s->value = 0;
  s->fun = (function*)0;
  return s;
}

cons *chareq(base_char *a, base_char *b)
{
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;//TODO error?
  else if (a->type != BASE_CHAR)
    return nil;//TODO error!
  else if (a->c == b->c)
    return t;
  else
    return nil;
}

cons *charequal(base_char *a, base_char *b)
{
  char ac;
  char bc;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;//TODO error?
  else if (a->type != BASE_CHAR)
    return nil;//TODO error!

  ac = a->c;
  if (ac<='z' && ac>='a')
    ac = ac-'a';

  bc = b->c;
  if (bc<='z' && bc>='z')
    bc = bc-'a';

  else if (ac == bc)
    return t;
  else
    return nil;
}

cons *stringeq(array *a, array *b)
{
  long i=0;
  if(a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  // else if (a->type != STRING)
  //  return nil;
  while(1)
    {
      if (chareq((base_char*)a->a[0][i], (base_char*)b->a[0][i]) == nil)
	return nil;
      else if (a->a[0][i] == 0) 
	return t;
      else
	i++;
    }
}

cons *stringequal(array *a, array *b)
{
  long i=0;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  //else if (a->type != STRING)
  //  return nil;
  while(1)
    {
      if (charequal((base_char*)a->a[0][i], (base_char*)b->a[0][i]) == nil)
	return nil;
      else if (a->a[0][i] == 0) 
	return t;
      else
	i++;
    }
}

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

//Equality checkers
cons *eq (cons *a, cons *b)
{
  if (a==b)
    return t;
  else
    return nil;
}

cons *eql (cons *a, cons *b)
{
  if (a == b)
    return t;
  else if (a->type == b->type)
    {
      switch (a->type)
	{
	  //numbers
	case (FIXNUM):
	  if (((fixnum*)a)->num == ((fixnum*)b)->num)
	    return t;
	case (BIGNUM):
	  while (((bignum*)a)->num == ((bignum*)b)->num)
	    {
	      if (((cons*)((bignum*)a)->next == nil) && 
		  ((cons*)((bignum*)b)->next == nil))
		return t;
	      a = (cons*)((bignum*)a)->next;
	      b = (cons*)((bignum*)b)->next;
	    }
	  return nil;
	case (RATIO):
	  if ((((ratio*)a)->numerator == ((ratio*)b)->numerator) &&
	      (((ratio*)a)->denominator == ((ratio*)b)->denominator))
	    return t;
	  else
	    return nil;
	case (SINGLE):
	  if ((((single*)a)->sign == ((single*)b)->sign) &&
	      (((single*)a)->base == ((single*)b)->base) &&
	      (eql((cons*)((single*)a)->exponent, (cons*)((single*)b)->exponent) == t) &&
	      (eql((cons*)((single*)a)->integer, (cons*)((single*)b)->integer)))
	    return t;
	  else
	    return nil;
	  //characters
	case (BASE_CHAR):
	  if (((base_char*)a)->c == ((base_char*)b)->c)
	    return t;
	  else
	    return nil;
	default:
	  return nil;
	}
    }
  else
    return nil;
}

//Lookup a symbol in the current environment; can't do foreign packages yet. Wait 'till read() gets done.
cons *lookup(char *namestr, cons *env)
{
  array *name = strtolstr(namestr);
  symbol *s = intern(name, (package*)((symbol*)((procinfo*)env->car)->package_sym)->value);
  return eval((cons*)s, env);
}

//Create an association pair
cons *mkpair(cons *key, cons *value)
{
  cons *a = newcons();
  a->car = key;
  a->cdr = newcons();
  a->cdr->car = value;
  return a;
}

cons *eval(cons *exp, cons *env)
{
  if (exp == nil)
    return nil;
  else if (exp == t)
    return t;
  else if (exp->type == SYMBOL)
    {//
      symbol *s = (symbol*)exp;
      cons *c = env->cdr;
      //current environment node

      if (assoc((cons*)dynamic, s->plist) != nil)
	{
	  if (s->value != 0)
	    return s->value;
	  else
	    return nil;//TODO no value error
	}
      
      while (c!=nil)
	{//Loop through the lexical environment
	  if ((symbol*)c->car->car == s)
	    return c->car->cdr->car;
	  else if (c->cdr == nil)
	    //If this env level is empty, go down one level
	    c = c->car->cdr;
	  else
	    c = c->cdr;
	}
      if (s->value != 0)
	return s->value;
      else
	return nil;//TODO no value error
      //If there's no binding in the lexical environment, return the dynamic binding.
    }
  else if ((exp->type == LIST) && 
	   (exp->car->type != LIST) &&
	   (assoc(exp->car->car, special_operators->value) == nil))
    {
      symbol *s = (symbol*)exp->car;
      function *f;
      if (exp->car->type == SYMBOL)
	{
	  f = (function*)s->fun;
	  if (f == (function*)0)
	    return nil;//TODO error no function binding
	}
      else if ((exp->car->type == FUNCTION) ||
	       (exp->car->type == COMPILED_FUNCTION))
	f = (function*)exp->car;
      
      if (f->type == FUNCTION)
	{
	  cons *newenv = extend_env(f->env);
	  newenv = evalambda(f->lambda_list, exp->cdr, newenv);
	  return eval(f->fun, evalambda(f->lambda_list, exp->cdr, newenv));
	}
      else if (f->type == COMPILED_FUNCTION)
	{
	  compiled_function *cf = (compiled_function*)f;
	  cons *newenv = extend_env(cf->env);
	  newenv = evalambda(cf->lambda_list, exp->cdr, newenv);
	  return cf->fun(newenv);
	}
      //though garbled, the previous just calls a C function pointer.
      else
	return nil;//TODO error, not a function
    }
 else if ((exp->type == LIST) && 
	   (exp->car->type != LIST) &&
	  (assoc(exp->car->car, (cons*)special_operators->value) == nil))
   {//TODO
     return 0; 
   }
  else
    return nil;//TODO should be error
}

cons *extend_env(cons *env)
{
  cons *oldenv = env;
  cons *newenv = newcons();
  newenv->car = oldenv->car;//procinfo
  newenv->cdr = newcons();
  newenv->cdr->car = oldenv;
  //Last entry in the lexical environment. Points to the previous environment "level".
  //New lexical bindings will be pushed in before this. When the evaluator, looking for bindings,
  //reaches a lexical binding whose cdr is nil, it will know to descend to the next level.
  //When both the car and cdr are nil, there's nowhere else to go.
  return newenv;
}

cons *envbind(cons *key, cons *value, cons *env)
{
  cons *first = env;  
  cons *newenv = newcons();
  newenv->car = fcons(key, fcons(value, nil));
  newenv->cdr = env->cdr;
  env->cdr = newenv;
  return env;
}

cons *evalambda(cons *lambda_list, cons *args, cons *env)
{//Clean this up. This is the worst code I've ever written. :(
  cons *oldenv = env;
  cons *binding;
  cons *foo;
  array *varname;
  symbol *varsym;
  
  while((null(lambda_list) == nil) && 
	(null(args) == nil) &&
	(lambda_list->car != (cons*)optional) &&//Break on 
	(lambda_list->car != (cons*)rest) &&
	(lambda_list->car != (cons*)keyword) &&
	(lambda_list->car != (cons*)aux))
    {
      //varname = ((symbol*)lambda_list->car)->name;
      //varsym = intern(varname, (package*)((symbol*)((procinfo*)env->car)->package_sym)->value);
      varsym = (symbol*)lambda_list->car;
      envbind((cons*)varsym, eval(args->car, env), env);
      
      lambda_list = lambda_list->cdr;
      args = args->cdr;
    }

  if (lambda_list->car != (cons*)optional)
    {
      lambda_list = lambda_list->cdr;
      while((null(lambda_list) == nil) && 
	    (null(args) == nil) &&
	    (lambda_list->car != (cons*)rest) &&
	    (lambda_list->car != (cons*)keyword) &&
	    (lambda_list->car != (cons*)aux))
	{//While there are values for the optional arguments...
	  if (lambda_list->car->type == SYMBOL)
	    varsym = (symbol*)lambda_list->car;
	  else if ((lambda_list->car->type == LIST) &&
		   (lambda_list->car->car->type == SYMBOL))
	    varsym = (symbol*)lambda_list->car->car;
	  else
	    return 0;//TODO error

	  envbind((cons*)varsym, eval(args->car, env), env);

	  lambda_list = lambda_list->cdr;
	  args = args->cdr;
	}
      while((null(lambda_list) == nil) && 
	    (null(args) == t) &&
	    (lambda_list->car != (cons*)rest) &&
	    (lambda_list->car != (cons*)keyword) &&
	    (lambda_list->car != (cons*)aux))
	{//If we've run out of args, initialize the optionals to their default values.
	  if (lambda_list->car->type == SYMBOL)
	    {
	      varsym = (symbol*)lambda_list->car;
	      envbind((cons*)varsym, nil, env);
	      lambda_list = lambda_list->cdr;
	      args = args->cdr;
	    }
	  else if ((lambda_list->car->type == LIST) &&
		   (lambda_list->car->car->type == SYMBOL))
	    {
	      varsym = (symbol*)lambda_list->car->car;
	      envbind((cons*)varsym, eval(lambda_list->car->cdr, env), env);
	      lambda_list = lambda_list->cdr;
	      args = args->cdr;
	    }
	  else
	    return 0;//TODO error
	}
    }
  if (lambda_list->car != (cons*)rest)
    //    env = evalrest(lambda_list, args, env);
    {
      lambda_list = lambda_list->cdr;
  
      if((null(lambda_list) == nil) && 
	 (null(args) == nil) &&
	 (lambda_list->car != (cons*)optional) &&
	 (lambda_list->car != (cons*)rest) &&
	 (lambda_list->car != (cons*)keyword) &&
	 (lambda_list->car != (cons*)aux))
	{
	  if (lambda_list->car->type == SYMBOL)
	    varsym = (symbol*)lambda_list->car;
	  else
	    return 0;//TODO error

	  envbind((cons*)varsym, args, env);
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
{
  while(plist != nil)
    {
      if (key == plist->car->car)
	return plist->car;
      else
	plist  = plist->cdr;
    }
  return nil;
}

symbol *defun(symbol *sym, cons *lambda_list, cons *form, cons *env)
{
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
{
  base_char *to_ret = (base_char*)str->rv->a[0][str->read_index];
  (base_char*)str->rv->a[0][str->read_index] == 0;
  if (str->read_index < str->rv->length->num)
    str->read_index++;
  else if (str->rv->next != 0)
    {
      str->rv = str->rv->next;
      str->read_index = 0;
    }
  else
    return (base_char*)nil;//TODO reader error
  return to_ret;
}

base_char *peek_char(stream *str)
{
  return (base_char*)str->rv->a[0][str->read_index];
}

cons *unread_char(base_char *c, stream *str)
{
  if (str->read_index>1)
    str->read_index--;
  else
    {
      array *old_rv = str->rv;
      str->rv = newsimple_vector(10);
      //Add a small new vector, and assume we won't be unreading many chars.
      str->rv->next = old_rv;
      str->read_index = 9;
      str->rv->a[0][9] = (cons*)c;
    }    
  str->rv->a[0][str->read_index] = (cons*)c;
  return nil;
}

//Abandon all hope, ye who enter here.
//Here be dragons...
//The READER function!

int main ()
{
  procinfo *proc = init();
  
  /*Tests*/
  cons *env = extend_env(nil);
  env->car = (cons*)proc;

  cons *exp = fcons((cons*)quote, fcons((cons*)quote, nil));

  cons *hope = eval(exp, env);

  stream *str = malloc(sizeof(stream));
  str->read_index = 0;
  str->rv = strtolstr("(*READTABLE*) ");
  str->write_index = 20;

  //cons *xyzzy = (cons*)read(str, env);
  //cons *wow = eval(xyzzy, env);
  cons *lesser = eval((cons*)readtable, env);

  return 0;
}
