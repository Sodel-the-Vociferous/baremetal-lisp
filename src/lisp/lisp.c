#include <stdlib.h>
#include "lisp.h"

cons t_phys;
cons nil_phys;

cons *t = &t_phys;
cons *nil = &nil_phys;


void *init()
{
  t->type = T;
  
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
}

cons *numberp(cons *a)
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

cons *fcons(cons *a, cons *b)
{
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

cons *cdr(cons *a)
{
  if (a->type == CONS)
    return a->cdr;
  else
    return nil;//TODO error
}

cons *eq (cons *a, cons *b)
{
  if (a==b)
    return t;
  else
    return nil;
}

cons *eql (cons *a, cons *b)
{
  if (eq(a, b) == t)
    return t;
  else if (a->type == b->type)
    {
      switch (a->type)
	{
	  //numbers
	case (FIXNUM):
	  if (a->num = b->num)
	    return t;
	case (BIGNUM):
	  while (a->num = b->num)
	    {
	      if ((a->next == nil) && 
		  (b->next = nil))
		return t;
	      a = a->next;
	      b = b->next;
	    }
	  return nil;
	case (RATIO):
	  if ((a->numerator == b->numerator) && 
	      (a->denominator = b->denominator))
	    return t;
	  else
	    return nil;
	case (SINGLE):
	  if ((a->sign == b->sign) &&
	      (a->base == b->base) &&
	      (eql(a->exponent, b->exponent) == t) &&
	      (eql(a->integer, b->integer)))
	    return t;
	  else
	    return nil;
	  //characters
	case (BASE_CHAR):
	  if (a->c == b->c)
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

cons *equal (cons *a, cons *b)
{//AHHHHHHHHHHHHHHH TODOOOOOOOO
  if (eq(a, b) == t)
    return t;
  else
    return nil;
}
  
