#include "lisp.h"
#include "init.h"

cons *read_token(stream*, cons*);
cons *interpret_token(char[], cons*)cons *interpret_token(char token[], cons *env);
{/* This entire function is non-conformant to the CLHS specified interpretation
  * of tokens. This is just here to get off the ground, and supports only simple
  * integers, and symbols.
  */
  return 0;//TODO
}

cons *read_token(stream *str, cons *env)
{
  char token[128];
  char token_index = 0;
  cons *plist;
  array *readtable = readtable_s->value;//TODO check w/ (readtablep...)
  base_char *c;

  while (1)
    {
      c = read_char(str);
      plist = assoc(c, readtable);
      if ((null(assoc(constituent, plist)) == nil) ||
	  (null(assoc(whitespace, plist)) == nil))
	{//TODO expand as per CLHS 2.2 step 8
	  if (token_index >= 128)
	    return 0; //TODO error handler
	  token[token_index] = c->c;
	  token_index++;
	}
      else if (null(assoc(single_escape, plist)) == nil)
	{
	  c = read_char(str);
	  if (token_index >= 128)
	    return 0; //TODO error handler
	  token[token_index] = c->c;
	  token_index++;
	}
      else if (null(assoc(multiple_escape, plist)) == nil)
	{//TODO mult esc
	  while (1)
	    {
	      if ((null(assoc(constituent, plist)) == nil) ||
		  (null(assoc(whitespace, plist)) == nil))
		{//TODO expand as per CLHS 2.2 step 9
		  if (token_index >= 128)
		    return 0; //TODO error handler
		  token[token_index] = c->c;
		  token_index++;
		}
	      else if (null(assoc(single_escape, plist)) == nil)
		{
		  c = read_char(str);
		  if (c == nil)
		    return 0;//TODO eof-error
		  else if (token_index >= 128)
		    return 0; //TODO error handler
		  
		  token[token_index] = c->c;
		  token_index++;
		}
	      else if (null(assoc(multiple_escape, plist)) == nil)
		break;
	      else if (null(assoc(invalid, plist)) == nil)
		return 0;//TODO reader error
	      else 
		/* This should never happen, and signals a problem with the
		 * interpreter's code.
		 */
		return 0;//TODO error
	    }
	}
      else if (null(assoc(invalid, plist)) == nil)
	return 0;//TODO reader error
      else if ((null(assoc(terminating_macro_char, plist)) == nil)
	       (null(assoc(whitespace, plist)) == nil))
	{
	  //TODO if (read_preserving_whitespace_s->value != nil)
	  unread_char(c, str);
	  break;
	}
      else
	/* This should never happen, and signals a problem with the
	 * interpreter's code.
	 */
	return 0;//TODO error
    }
  return interpret_token(token, env);
}


cons *read(stream *str, cons *env)
{
  array *readtable = readtable_s->value;//TODO check w/ (readtablep...)
  function *fun;
  cons *plist;
  while (1)
    {
      base_char *c = read_char(str);
      plist = assoc(c, readtable);
      if (c == nil)
	return 0;//TODO EOF processing
      else if (null(assoc(invalid, plist)) == nil)
	return 0;//TODO reader error
      else if (null(assoc(whitespace, plist)) == nil)
	c = read_char(str);
      else if (null((fun = assoc(terminating_macro_char, plist))) == nil)
	{
	  cons *exp = fcons((cons*)fun, 
			    fcons((cons*)str, 
				  fcons((cons*)c, 
					nil)));
	  return eval(exp, env);
	}
      else if (null((fun = assoc(non_terminating_macro_char, plist))) == nil)
	{
	  cons *exp = fcons((cons*)fun, 
			    fcons((cons*)str, 
				  fcons((cons*)c, 
					nil)));
	  return eval(exp, env);
	}
      else if ((null(assoc(single_escape, plist)) == nil) ||
	       (null(assoc(multiple_escape, plist)) == nil))
	{
	  unread_char(c, str);
	  return read_token(str, env);
	}
      else if (null(assoc(multiple_escape, plist)) == nil)
	{
	  unread_char(c, str);
	  return read_token(str, env);
	}
      else if (null(assoc(constituent, plist)) == nil)
	{
	  unread_char(c, str);
	  return read_token(str, env);
	}
}
