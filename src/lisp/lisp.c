#include <stdlib.h>
#include "lisp.h"
#include "init.h"

cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;

//Object allocation routines
cons *newcons()
{
  cons *c = malloc(sizeof(cons));
  c->type = CONS;
  c->car = nil;
  c->cdr = nil;
  return c;
}

fixnum *newfixnum()
{
  fixnum *f = malloc(sizeof(fixnum));
  f->type = FIXNUM;
  f->num = 0;
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

simple_vector *newsimple_vector(int size)
{
  simple_vector *v = malloc(sizeof(simple_vector));
  int i;
  v->type = SIMPLE_VECTOR;
  v->plist = nil;
  v->size = size;
  v->datatype = T;
  v->v = malloc(((size) * sizeof(cons*)));
  for (i=0;i<size;i++)
    v->v[i] = nil;
  v->next = (simple_vector*)0;
  return v;
}

package *newpackage()
{
  package *p = malloc(sizeof(package));
  p->type = PACKAGE;
  p->plist = nil;
  p->name = (simple_vector*)nil;
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

//Helper functions to translate C stuff into Lisp objects

base_char *ctolc(char c)
{//Char to Lisp Charx
  base_char *lc = newbase_char();
  lc->c = c;
  return lc;
}

simple_vector *strtolstr(char *str)
{//C string to Lisp string.
  int string_len;
  int i;
  for(string_len=1;*(str+string_len-1)!=0;string_len++);
  //Find the string length.

  simple_vector *to_ret = newsimple_vector(string_len);
  to_ret->type = STRING;
  to_ret->datatype = BASE_CHAR;
 
  base_char *c = 0;
  for(i=0;*str!=0;i++)
    {//Add the base_chars to the string simple_vector.
      c = newbase_char();
      to_ret->v[i] = (cons*)c;
      c->c = *str;
      str++;
    }
  c = newbase_char();
  to_ret->v[i] = (cons*)c;
  c->c = 0;
  return to_ret;
}

//Primitives
cons *null (cons *a)
{//Is object nil?
  if (a == nil ||
      (a->type = CONS &&
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
  to_ret->type = CONS;  
  to_ret->car = a;
  to_ret->cdr = b;
  return to_ret;
}

cons *car(cons *a)
{
  if (a->type == CONS)
    return a->car;
  else
    return nil;//TODO error
}

cons *rplaca(cons *a, cons *new)
{
  if (a->type == CONS && a != nil)
    {
      a->car = new;
      return a;
    }
  else
    return nil;//TODO error
}

cons *cdr(cons *a)
{
  if (a->type == CONS)
    return a->cdr;
  else
    return nil;//TODO error
}

cons *rplacd(cons *a, cons *new)
{
  if (a->type == CONS && a != nil)
    {
      a->cdr = new;
      return a;
    }
  else
    return nil;//TODO error
}

/*This is all infrastructure for the intern function. */
symbol *intern(simple_vector *name, package *p)
{//HARK. This function doesn't do symbol lookups in other packages with the : and :: syntax. Change this later. :]
  int i;
  char hashed_name[4];
  for (i=0;i<3;i++)
    {
      if (i >= name->size)
	hashed_name[i] = 0;
      else
	hashed_name[i] = ((base_char*)name->v[i])->c;
    }
  unsigned int index = *(unsigned int*)&hashed_name[0] % HASH_TABLE_SIZE;
  //Hash the first four characters of the name.
  
  cons *entry = p->global->v[index];
  symbol *s;

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
	    entry = entry->cdr;//next entry
	}
    }
  else if (entry == nil)
    {//Create a new entry for the hash value
      p->global->v[index] = newcons();
      p->global->v[index]->car = (cons*)malloc(sizeof(symbol));
      entry = p->global->v[index];
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

cons *stringeq(simple_vector *a, simple_vector *b)
{
  int i=0;
  if(a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  // else if (a->type != STRING)
  //  return nil;
  while(1)
    {
      if (chareq((base_char*)a->v[i], (base_char*)b->v[i]) == nil)
	return nil;
      else if (a->v[i] == 0) 
	return t;
      else
	i++;
    }
}

cons *stringequal(simple_vector *a, simple_vector *b)
{
  int i=0;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  //else if (a->type != STRING)
  //  return nil;
  while(1)
    {
      if (charequal((base_char*)a->v[i], (base_char*)b->v[i]) == nil)
	return nil;
      else if (a->v[i] == 0) 
	return t;
      else
	i++;
    }
}

package *find_package(simple_vector *name, procinfo *pinfo)
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
  simple_vector *name = strtolstr(namestr);
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
    {
      //symbol *s = intern((((symbol*)exp)->name), (package*)((symbol*)((procinfo*)env->car)->package_sym)->value);
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
  else if ((exp->type == CONS) && 
	   (exp->car->type != CONS))
    {
      //symbol *s = intern((((symbol*)exp->car)->name), (package*)((symbol*)((procinfo*)env->car)->package_sym)->value);
      
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

cons *envbind(cons *env, cons *binding)
{
  cons *first = env;  
  cons *newenv = newcons();
  newenv->car = binding;
  newenv->cdr = env->cdr;
  env->cdr = newenv;
  return env;
}

//This needs to be updated, in accordance with new environment infrastructure.
cons *evalambda(cons *lambda_list, cons *args, cons *env)
{
  cons *oldenv = env;
  simple_vector *varname;
  symbol *varsym;
  cons *binding;
  
  while((null(lambda_list) == nil) && (null(args) == nil))
    {
      varname = ((symbol*)lambda_list->car)->name;
      varsym = intern(varname, (package*)((symbol*)((procinfo*)env->car)->package_sym)->value);
      
      binding = newcons();
      binding->car = (cons*)varsym;
      binding->cdr = newcons();
      binding->cdr->car = args->car;

      envbind(env, binding);

      lambda_list = lambda_list->cdr;
      args = args->cdr;
    }
  if (lambda_list != nil)
    return oldenv;//TODO error too few args
  else if (args != nil)
    return oldenv;//TODO error too many args
  else
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


//Stream functions
base_char *read_char(stream *str)
{
  base_char *to_ret = (base_char*)str->rv->v[str->read_index];
  (base_char*)str->rv->v[str->read_index] == 0;
  if (str->read_index < str->rv->size)
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
  return (base_char*)str->rv->v[str->read_index];
}

cons *unread_char(base_char *c, stream *str)
{
  if (str->read_index>1)
    str->read_index--;
  else
    {
      simple_vector *old_rv = str->rv;
      str->rv = newsimple_vector(10);
      str->rv->next = old_rv;
      str->read_index = 9;
      str->rv->v[9] = (cons*)c;
    }    
  str->rv->v[str->read_index] = (cons*)c;
  return nil;
}

//Abandon all hope, ye who enter here.
//Here be dragons...
//The READER function!


int main ()
{
  procinfo *proc = init();
  
  //Test
  cons *env = extend_env(nil);
  env->car = (cons*)proc;

  cons *exp = fcons((cons*)quote, fcons((cons*)quote, nil));

  cons *hope = eval(exp, env);

  stream *str = malloc(sizeof(stream));
  str->read_index = 0;
  str->rv = strtolstr("(*READTABLE*) ");
  str->write_index = 20;

  cons *xyzzy = read(str, env);
  //cons *wow = eval(xyzzy, env);
  //cons *lesser = eval((cons*)readtable, env);

  return 0;
}
