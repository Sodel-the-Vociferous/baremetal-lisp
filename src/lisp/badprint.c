#include "lisp.h"
#include "init.h"

cons *badprint_backend(cons *exp, stream *str)
{
  if (exp == nil)
    badprint_backend((cons*)nil_s, str);
  else if (exp == t)
    badprint_backend((cons*)t_s, str);
  else if (typep(exp, fixnum_s) == t)
    {
      unsigned long magnitude = 1000000000;
      long num = ((fixnum*)exp)->num;
      long i;
      char negative = 0;
      
      if (num < 0)
	{//TODO fix; negative numbers don't print properly.
	  negative = 1;
	  i = -num;
	}
      else if (num == 0)
	{
	  write_char(ctolc('0'), str);
	  return;
	}
      else
	i = num;
      
      while (magnitude != 0)
	{
	  if (num >= magnitude)
	    {
	      char n = (((num / magnitude) % 10) + '0');
	      write_char(ctolc(n), str);
	    }
	  magnitude = magnitude / 10;
	}
    }
  else if (typep(exp, ratio_s) == t)
    {
      ratio *r = (ratio*)exp;
      badprint_backend((cons*)r->numerator, str);
      write_char(ctolc('/'), str);
      badprint_backend((cons*)r->denominator, str);
    }
  else if (typep(exp, symbol_s) == t)
    {
      symbol *s = (symbol*)exp;
      if (s->home_package == keyword_pkg)
	 write_char(ctolc(':'), str);
      badprint_backend((cons*)s->name, str);
    }
  else if (typep(exp, string_s) == t)
    {
      array *s = (array*)exp;
      int i = 0;
      while (s != (array*)0)
	{
	  for (i=0; i<s->length->num;i++)
	    write_char((base_char*)s->a[0][i], str);
	  s = s->next;
	}
    }
  else if (typep(exp, list_s) == t)
    {
      write_char(ctolc('('), str);
      cons *foo = exp;
      if (foo == nil)
	{
	  write_char(ctolc(')'), str);
	  return;
	}

      while (1)
	{
	  badprint_backend(foo->car, str);
	  if (foo->cdr == nil)
	    {
	      write_char(ctolc(')'), str);
	      return;
	    }
	  else if (typep(foo->cdr, list_s) == t)
	    {
	      write_char(ctolc(' '), str);
	      foo = foo->cdr;
	    }
	  else
	    {
	      write_char(ctolc(' '), str);
	      write_char(ctolc('.'), str);
	      write_char(ctolc(' '), str);
	      badprint_backend(foo->cdr, str);
	      write_char(ctolc(')'), str);
	      return;
	    }
	}
    }
}

cons *badprint(cons*exp, stream *str)
{
  badprint_backend(exp, str);
  write_char(ctolc(' '), str);
}
