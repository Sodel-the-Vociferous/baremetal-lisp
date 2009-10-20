#include <stdlib.h>
#include "lisp.h"

cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;

cons *init()
{
  char cl_name[] = "COMMON-LISP";
  char cl_user_name[] = "COMMON-LISP-USER";
  char keyword_name[] = "KEYWORD";

  t->type = T;
  
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
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
  b->next = nil;
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
  c->0;
}

vector *newvector(int size)
{
  vector *v = malloc(sizeof(vector));
  v->type = VECTOR;
  v->plist = nil;
  v->size = size;
  v->datatype = t;
  v->v = malloc((size * sizeof(cons*)));
  v->next = nil;
}

vector *strtolstr(char *str)
{
  int string_len;
  int i;
  for(string_len=1;*(str+string_len)!=0;string_len++);

  vector *to_ret = newvector(string_len);
  to_ret->datatype = BASE_CHAR;
 
  base_char *c = 0;
  for(i=0;*str!=0;i++)
    {
      c = newbase_char();
      to_ret->v[i] = c;
      c->c = *str;
      str++;
    }
  to_ret->next = nil;
  return to_ret;
}

cons *null (cons *env)
{
  cons *a = car(env);
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

cons *fcons(cons *env)
//The cons function. f added to prevent name collision.
{
  cons *to_ret = malloc(sizeof(cons));
  to_ret->type = CONS;  
  to_ret->car = a;
  to_ret->cdr = b;
  return to_ret;
}

cons *car(cons *env)
{
  if (a->type == CONS)
    return a->car;
  else
    return nil;//TODO error
}

cons *rplaca(cons *env)
{
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
  if (a->type == CONS)
    return a->cdr;
  else
    return nil;//TODO error
}

cons *rplacd(cons *env)
{
  if (a->type == CONS && a != nil)
    {
      a->cdr = new;
      return a;
    }
  else
    return nil;//TODO error
}

cons *intern(vector *name, package *p)
{
  int i;
  int index = *(int*)name % HASH_TABLE_SIZE;
  cons *entry = p->global->table[index];
  symbol *s = (symbol*)entry->car;
}

cons kchareq(base_char *a, base_char *b)//for kernel use; doesn't use Lisp conventions
{
  
      

cons *eq (cons *env)
{
  if (a==b)
    return t;
  else
    return nil;
}
      
cons *eql (cons *env)
{
  if (eq(a, b) == t)
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

cons *equal (cons *env)
{//AHHHHHHHHHHHHHHH TODOOOOOOOO
  if (eq(a, b) == t)
    return t;
  else
    return nil;
}

  
int main ()
{
  cons *global = init();
  return 0;
}
