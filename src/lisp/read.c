/* read.c
 *
 * Daniel Ralston, 2009
 *
 * This code is released under the GNU GPL General Public License.
 */

#include "lisp.h"
#include "init.h"

cons *read_token(stream *str, cons *env)
{/* CLHS 2.2 Step 8
  */
  //TODO add support for floating point numbers, and bignums.
  //TODO make buffer overrun check robust.
  base_char *c;
  cons *readtable = readtable_s->value;//TODO check w/ (readtablep...)
  package *p = (package*)package_s->value;
  fixnum f = {.type=(cons*)FIXNUM, .num=(long)0};
  bignum b = {.type=(cons*)BIGNUM, .num=(long)0, .next=(bignum*)0};
  int magnitude = 1;
  char n;
  ratio r = {.type=(cons*)RATIO, .numerator=0, .denominator=0};
  single s;//{type=SINGLE, sign=0, base=newfixnum
  char negative = 0;
  char symbol_name[256];
  char symbol_idx = 0;
  short max_idx = 255;
  char external_required = 1;
  cons *foo;
  char base = ((fixnum*)read_base_s->value)->num;
  cons *plist;

  while (1)
    {
      c = read_char(str);
      plist = assoc((cons*)c, (cons*)readtable)->cdr;

      if (c == (base_char*)nil)
	{
	  if (symbol_idx > max_idx)
	    return (cons*)0xbad2;//TODO error. Must make robust!
	  symbol_name[symbol_idx] = '\0';
	  symbol_idx++;
	  break;
	}
      else if (null(assoc((cons*)package_marker, plist)->cdr) == nil) 
	{
	  f.type = (cons*)0;
	  b.type = (cons*)0;
	  s.type = (cons*)0;
	  
	  if (symbol_idx == 0)
	    {
	      p = keyword_pkg;
	      external_required = 0;
	    }
	  else
	    {
	      symbol_idx = 0;
	      p = find_package(strtolstr(symbol_name), (procinfo*)env->car);
	    }
	  
	  c = read_char(str);
	  plist = assoc((cons*)c, (cons*)readtable)->cdr;

	  if (null(assoc((cons*)package_marker, plist)->cdr) == nil) 
	    {
	      read_char(str);
	      external_required = 0;
	    }
	}
      if ((null(assoc((cons*)constituent, plist)->cdr) == nil) ||
	  (null(assoc((cons*)non_terminating_macro, plist)->cdr) == nil))
	{
	  if (symbol_idx > 255)
	    return (cons*)0xbad1;//TODO error. Must make robust!
	  symbol_name[symbol_idx] = c->c;
	  symbol_idx++;
	}
      else if (null(assoc((cons*)single_escape, plist)->cdr) == nil)
	{
	  c = read_char(str);
	  if (symbol_idx > max_idx)
	    return (cons*)0xbad2;//TODO error. Must make robust!
	  symbol_name[symbol_idx] = c->c;
	  symbol_idx++;
	  f.type = (cons*)0;
	  b.type = (cons*)0;
	  s.type = (cons*)0;
	}
      else if (null(assoc((cons*)multiple_escape, plist)->cdr) == nil)
	{/* CLHS 2.2 Step 9
	  */
	  f.type = (cons*)0;
	  b.type = (cons*)0;
	  s.type = (cons*)0;

	  while (1)
	    {
	      c = read_char(str);
	      plist = assoc((cons*)c, (cons*)readtable)->cdr;
	      
	      if (c == (base_char*)nil)
		return (cons*)0xbad14;
	      else if ((null(assoc((cons*)constituent, plist)->cdr) == nil) ||
		  (null(assoc((cons*)whitespace, plist)->cdr) == nil))
		{//TODO expand as per CLHS 2.2 step 9
		  if (symbol_idx > max_idx)
		    return (cons*)0xbad3;//TODO error. Must make robust!
		  symbol_name[symbol_idx] = c->c;
		  symbol_idx++;
		}
	      else if (null(assoc((cons*)single_escape, plist)->cdr) == nil)
		{
		  c = read_char(str);
		  if (c == (base_char*)nil)
		    return (cons*)0xbad4;//TODO eof-error
		  if (symbol_idx > max_idx)
		    return (cons*)0xbad5;//TODO error. Must make robust!
		  symbol_name[symbol_idx] = c->c;
		  symbol_idx++;
		}
	      else if (null(assoc((cons*)multiple_escape, plist)->cdr) == nil)
		break;
	      else if (null(assoc((cons*)invalid, plist)->cdr) == nil)
		return (cons*)0xbad6;//TODO reader error
	      else 
		/* This should never happen, and signals a problem with the
		 * interpreter's code.
		 */
		return (cons*)0xbad7;//TODO error
	    }
	}
      else if (null(assoc((cons*)terminating_macro, plist)->cdr) == nil)
	{
	  //TODO if (read_preserving_whitespace_s->value != nil) yadda yadda....
	  unread_char(c, str);

	  if (symbol_idx > 255)
	    return (cons*)0xbad14;//TODO error. Must make robust!
	  symbol_name[symbol_idx] = '\0';
	  symbol_idx++;
	  break;
	}
      else if (null(assoc((cons*)whitespace, plist)->cdr) == nil)
	{//TODO expand as per CLHS.
	  if (symbol_idx > 255)
	    return (cons*)0xbad14;//TODO error. Must make robust!
	  symbol_name[symbol_idx] = '\0';
	  symbol_idx++;
	  break;
	}
     else if (null(assoc((cons*)invalid, plist)->cdr) == nil)
	return (cons*)0xbad8;//TODO reader error
      else
	/* This should never happen, and signals a problem with the
	 * interpreter's code.
	 */
	return (cons*)0xbad9;//TODO error

      if (f.type != (cons*)0)
	{
	  if ((c->c >= '0') &&
	      (c->c <= '9'))
	    n = c->c - '0';
	  else if ((c->c >= 'A') &&
		   (c->c <= 'Z'))
	    n = (c->c - 'A' + 9);
	  
	  if (n >= base)
	    {
	      f.type = (cons*)0;
	      b.type = (cons*)0;
	    }
	  f.num *= base;
	  f.num += n;
	}
      if (b.type != (cons*)0)
	//TODO bignum
	continue;
      if (s.type != (cons*)0)
	//TODO float
	continue;    
    }
  if (f.type != (cons*)0)
    {
      foo = (cons*)newfixnum(0);
      *(fixnum*)foo = f;
    }
  else
    {
      foo = (cons*)intern(strtolstr(symbol_name), p);
      
      if ((external_required = 1) &&
	  (null(assoc((cons*)external, ((symbol*)foo)->plist)->cdr) == t) &&
	  (p != (package*)package_s->value))
	return (cons*)0xbad10;//TODO non-external error
    }
  return foo;
}

cons *read(stream *str, cons *env)
{
  cons *readtable = readtable_s->value;//TODO check w/ (readtablep...)
  function *fun;
  cons *plist;

  base_char *c = read_char(str);
  plist = assoc((cons*)c, (cons*)readtable)->cdr;

  if (c == (base_char*)nil)
    return (cons*)0xbad11;//TODO EOF processing
  else if (null(assoc((cons*)constituent, plist)->cdr) == nil)
    {
      unread_char(c, str);
      return read_token(str, env);
    }
  else if (null(assoc((cons*)whitespace, plist)->cdr) == nil)
    c = read_char(str);
  else if ((null(assoc((cons*)single_escape, plist)->cdr) == nil) ||
	   (null(assoc((cons*)multiple_escape, plist)->cdr) == nil))
    {
      unread_char(c, str);
      return read_token(str, env);
    }
  else if (null((cons*)(fun = (function*)assoc((cons*)terminating_macro, plist)->cdr)) == nil)
    {
      if (c->c == ')')
	/* This is brittle. It's just a magic number to tell read_list() that 
	 * the list is over.This WILL be replaced when condition handling is 
	 * introduced. 
	 */
	return (cons*)0xBADB0B;
      cons *exp = fcons((cons*)fun, 
			fcons((cons*)str, 
			      fcons((cons*)c, 
				    nil)));
      return eval(exp, env);
    }
  else if (null((cons*)(fun = (function*)assoc((cons*)non_terminating_macro, plist)->cdr)) == nil)
    {
      cons *exp = fcons((cons*)fun, 
			fcons((cons*)str, 
			      fcons((cons*)c, 
				    nil)));
      return eval(exp, env);
    }
  else if (null(assoc((cons*)multiple_escape, plist)->cdr) == nil)
    {
      unread_char(c, str);
      return read_token(str, env);
    }
  else if (null(assoc((cons*)invalid, plist)->cdr) == nil)
    return (cons*)0xbad12;//TODO reader error
  else
    /* This should never happen, and signals a problem with the 
     * interpreter's code.
     */
    return (cons*)0xBADD09;//TODO error
}

cons *read_list(stream *str,  base_char *c, cons *env)
{
  cons *plist;
  cons *to_ret = nil;
  cons *current;
  cons *foo;
  cons *readtable = readtable_s->value;

  while (1)
    {	      
      foo = read(str, env);
      if (foo == (cons*)0xBADB0B)
	/* Signals we have run into a right-paren. To be replaced. See code for
	 * terminating macro characters.
	 */
	break;

      if (to_ret == nil)
	{
	  to_ret = newcons();
	  to_ret->car = foo;
	  current = to_ret;
	}
      else
	{
	  current->cdr = newcons();
	  current = current->cdr;
	  current->car = foo;
	}

      while (1)
	{
	  c = peek_char(str);
	  plist = assoc((cons*)c, (cons*)readtable)->cdr;
	  while (null(assoc((cons*)whitespace, plist)->cdr) == nil)
	    read_char(str);
	  if (null(assoc((cons*)dot, plist)->cdr) == nil)
	    {
	      foo = read(str, env);
	      current->cdr = foo;
	      if (read(str, env) != (cons*)0xBADB0B)
		return 0;//TODO error
	    }
	  else
	    break;
	}
    }
  return to_ret;
}
