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

  cons *package_list = malloc(sizeof(cons));
  cons *thiscons = package_list;
  package_list->type = CONS;
  package_list->car = malloc(sizeof(package));
  package_list->cdr = malloc(sizeof(cons));

  package *thispackage = (package*)package_list->car;
  thispackage->type = PACKAGE;
  thispackage->plist = nil;
  thispackage->name = strtolstr(&cl_name[0]);

  thiscons = cdr(thiscons);
  thiscons->type = CONS;
  thiscons->car = malloc(sizeof(package));
  thiscons->cdr = malloc(sizeof(cons));
  
  thispackage = (package*)car(thiscons);
  thispackage->type = PACKAGE;
  thispackage->plist = nil;
  thispackage->name = strtolstr(&cl_user_name[0]);
  thispackage->cdr = malloc(sizeof(cons));

  thiscons = cdr(thiscons);
  thiscons->type = CONS;
  thiscons->car = malloc(sizeof(package));
  thiscons->cdr = malloc(sizeof(cons));
  
  thispackage = (package*)car(thiscons);
  thispackage->type = PACKAGE;
  thispackage->plist = nil;
  thispackage->name = strtolstr(&keyword_name[0]);
  thispackage->cdr = malloc(sizeof(cons)); 
}

vector *strtolstr(char *str)
{
  int i;

  vector *to_ret = malloc(sizeof(vector));
  to_ret->plist = nil;

  for (i=1;*str!=0;i++);
  to_ret->size = i;
  to_ret->datatype = BASE_CHAR;
  to_ret->v = malloc((i*sizeof(base_char)));
  for(i=0;*str!=0;i++)
    {
      to_ret->v[i] = *str;
      str++;
    }
  to_ret->next = nil;
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
