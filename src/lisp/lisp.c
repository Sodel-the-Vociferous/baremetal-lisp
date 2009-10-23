#include <stdlib.h>
#include "lisp.h"

cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;

procinfo *init()
{
  vector *cl_name = strtolstr("COMMON-LISP");
  vector *cl_user_name = strtolstr("COMMON_LISP_USER");
  vector *keyword_name = strtolstr("KEYWORD");
  vector *nil_name = strtolstr("NIL");
  vector *t_name = strtolstr("T");

  package *cl = newpackage();
  cl->name = strtolstr((char*)&cl_name);

  package *cl_user = newpackage();
  cl_user->name = strtolstr((char*)&cl_user_name);

  package *keyword = newpackage();
  keyword->name = strtolstr((char*)&keyword_name);
  
  //init t
  t->type = T;
  symbol *t_sym = fintern(t_name, cl);
  t_sym->value = t;
  symbol *test = fintern(t_name, cl);
  
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;

  procinfo *main = malloc(sizeof(procinfo));
  main->type = PROCINFO;
  main->package  = 0;
  main->packages = newcons();
  main->packages->car = (cons*)cl;
  main->packages->cdr = newcons();
  main->packages->cdr->car = (cons*)cl_user;
  main->packages->cdr->cdr = newcons();
  main->packages->cdr->cdr->car = (cons*)keyword;  
  return main;
}

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

vector *newvector(int size)
{
  vector *v = malloc(sizeof(vector));
  int i;
  v->type = VECTOR;
  v->plist = nil;
  v->size = size;
  v->datatype = T;
  v->v = malloc(((size) * sizeof(cons*)));
  for (i=0;i<size;i++)
    v->v[i] = nil;
  v->next = (vector*)nil;
  return v;
}

package *newpackage()
{
  package *p = malloc(sizeof(package));
  p->type = PACKAGE;
  p->plist = nil;
  p->name = (vector*)nil;
  p->global = newvector(HASH_TABLE_SIZE);
  return p;
}

vector *strtolstr(char *str)
{
  int string_len;
  int i;
  for(string_len=1;*(str+string_len-1)!=0;string_len++);

  vector *to_ret = newvector(string_len);
  to_ret->datatype = BASE_CHAR;
 
  base_char *c = 0;
  for(i=0;*str!=0;i++)
    {
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

cons *null (cons *env)
{
  cons *a = lookup("A", env);
  if (a == nil ||
      (a->type = CONS &&
       a->car == nil &&
       a->cdr == nil))
    return t;
  else
    return nil;
}

cons *numberp(cons *env)
{
  cons *a = lookup("A", env);
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

//The cons function. f added to prevent name collision.
cons *fcons(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  cons *to_ret = malloc(sizeof(cons));
  to_ret->type = CONS;  
  to_ret->car = a;
  to_ret->cdr = b;
  return to_ret;
}

cons *car(cons *env)
{
  cons *a = lookup("A", env);
  if (a->type == CONS)
    return a->car;
  else
    return nil;//TODO error
}

cons *rplaca(cons *env)
{
  cons *a = lookup("A", env);
  cons *new = lookup("NEW", env);
  if (a->type == CONS && a != nil)
    {
      a->car = new;
      return a;
    }
  else
    return nil;//TODO error
}

cons *cdr(cons *env)
{
  cons *a = lookup("A", env);
  if (a->type == CONS)
    return a->cdr;
  else
    return nil;//TODO error
}

cons *rplacd(cons *env)
{
  cons *a = lookup("A", env);
  cons *new = lookup("NEW", env);
  if (a->type == CONS && a != nil)
    {
      a->cdr = new;
      return a;
    }
  else
    return nil;//TODO error
}

symbol *fintern(vector *name, package *p)
{//This is all infrastructure for the intern function. ('f' prefixed because this one uses
  //C calling convention.) One day, I will re-implement this in Lisp.
  //HARK. This function doesn't do symbol lookups in other packages with the : and :: syntax. Change this later. :]
  int i;
  char hashed_name[4];
  for (i=0;i<3;i++)
    {
      if (i >= name->size)
	hashed_name[i] = 0;
      else
	hashed_name[i] = ((base_char*)name->v[i])->c;
    }
  int index = *(int*)&hashed_name[0] % HASH_TABLE_SIZE;
  
  cons *entry = p->global->v[index];
  symbol *s;

  if (entry != nil)
    {
      s = (symbol*)entry->car;

      while(entry != nil)
	{
	  if (bstringequal(name, s->name) == t)
	    return s;
	  else if (entry->cdr == nil)
	    {
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
    {
      p->global->v[index] = newcons();
      p->global->v[index]->car = (cons*)malloc(sizeof(symbol));
      entry = p->global->v[index];
    }
  s = (symbol*)entry->car;
  s->type = SYMBOL;
  s->name = name;
  s->home_package = p;
  s->value = nil;
  s->function = nil;
  return s;
}

cons *bchareq(base_char *a, base_char *b)
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

cons *bcharequal(base_char *a, base_char *b)
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

cons *bstringeq(vector *a, vector *b)
{
  int i=0;
  if(a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  else if (a->type != STRING)
    return nil;
  while(1)
    {
      if (bchareq((base_char*)a->v[i], (base_char*)b->v[i]) == nil)
	return nil;
      else if (a->v[i] == 0) 
	return t;
      else
	i++;
    }
}

cons *bstringequal(vector *a, vector *b)
{
  int i=0;
  if (a==b)
    return t;
  else if (a->type != b->type)
    return nil;
  else if (a->type != STRING)
    return nil;
  while(1)
    {
      if (bcharequal((base_char*)a->v[i], (base_char*)b->v[i]) == nil)
	return nil;
      else if (a->v[i] == 0) 
	return t;
      else
	i++;
    }
}

//ACTUAL Lisp equality checkers
cons *eq (cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  if (a==b)
    return t;
  else
    return nil;
}
      
cons *eql (cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
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
	      (feql((cons*)((single*)a)->exponent, (cons*)((single*)b)->exponent) == t) &&
	      (feql((cons*)((single*)a)->integer, (cons*)((single*)b)->integer)))
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

cons *feql (cons *a, cons *b)
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
	      (feql((cons*)((single*)a)->exponent, (cons*)((single*)b)->exponent) == t) &&
	      (feql((cons*)((single*)a)->integer, (cons*)((single*)b)->integer)))
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

cons *lookup(char *namestr, cons *env)
{
  vector *name = strtolstr(namestr);
  symbol *s = fintern(name, (package*)((procinfo*)env->car)->package->value);
  return eval((cons*)s, env);
}

cons *eval(cons *exp, cons *env)
{
  if (exp == nil)
    return nil;
  else if (exp == t)
    return t;
  else if (exp->type == SYMBOL)
    {
      symbol *s = fintern((((symbol*)exp)->name), (package*)((procinfo*)env->car)->package->value);
      cons *c = env->cdr;//current environment node
      while (c!=nil)//Loop through the lexical environment
	{
	  if ((symbol*)c->car->car == s)
	    return c->car->cdr->car;
	  else if (c->cdr == nil)
	    c = c->car->cdr;
	  else
	    c = c->cdr;
	}
      return s->value;
    }
  else if ((exp->type == CONS) && 
	   (exp->car->type != CONS))
    {
      symbol *s = fintern((((symbol*)exp->car)->name), (package*)env->car);
      function *f = (function*)s->function;
      if (f == (function*)nil)
	return nil;//TODO error no function binding
      
      env = extend_env(env);
      env = evalambda(f->lambda_list, exp->cdr, env);
      if (f->type == FUNCTION)
	return eval(f->function, f->env);
      else if (f->type == COMPILED_FUNCTION)
	return (*(((compiled_function*)f)->function)) (f->env);
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
  newenv->car = oldenv->car;//Package list
  newenv->cdr = newcons();
  newenv->cdr->car = oldenv;
  //Last entry in the lexical environment. Points to the previous environment "level".
  //New lexical bindings will be pushed in before this. When the evaluator, looking for bindings,
  //reaches a lexical binding whose cdr is nil, it will know to descend to the next level.
  //When both the car and cdr are nil, there's nowhere else to go.
  env = newenv;
  return env;
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
  vector *varname;
  symbol *varsym;
  cons *binding;
  
  while((null(lambda_list) == nil) && (null(args) == nil))
    {
      varname = ((symbol*)lambda_list->car)->name;
      varsym = fintern(varname, (package*)env->car);
      
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
  
int main ()
{
  procinfo *main = init();
  return 0;
}
