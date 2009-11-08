#include <stdlib.h>
#include "lisp.h"

//Sizes are specifically for 64 bits! MUST CHANGE for 32 bit system or BREAKAGE.

struct pair *nil;
struct pair *t;
struct pair t_phys;
struct pair nil_phys;

void init ()
{
  //Initialize nil
  nil = &nil_phys;
  nil->type = LIST;
  nil->car = (void*)&nil_phys;
  nil->cdr = (void*)&nil_phys;

  //Initialize t
  t = &t_phys;
  t->type = T;
  t->car = (void*)0;
  t->cdr = (void*)0;
}

void *kallocate()
{
  //Allocate a single block, and zero it.
  pair *target = malloc(sizeof(pair));
  target->type = 0;
  target->car = 0;
  target->cdr = 0;
  return (void*)target;
}

//Check if a character is whitespace
pair *whitespacep (char c)
{
  if (c == ' ' || c == '\n' || c == '\t')
    return t;
  else
    return nil;
}

//Cobvert C string to Lisp string
string *strtolstr (char *str)
{
  string *s = kallocate();
  string *to_ret = s;
  int i;

  s->type = STRING;
  s->cdr = nil;

  while (1)
    {
      for (i=0;i<8;i++)
	{
	  //place four characters in each string object
	  s->text[i] = *str;
	  if (*str == 0)
	    //If it's the end of the string, return the string object
	    return to_ret;
	  str++;
	}
      //Add another string object to the end of our string
      s->cdr = (pair*)kallocate();
      s = (string*)s->cdr;
      s->type = STRING;
      s->cdr = nil;
    }
}

///Convert C string to Lisp symbol
symbol *strtosym (char *str)
{
  symbol *s = kallocate();
  symbol *to_ret = s;
  int i;

  s->type = SYMBOL;
  s->cdr = nil;

  while (1)
    {
      for (i=0;i<8;i++)
	{
	  //place four characters in each symbol object
	  s->text[i] = *str;
	  if (*str == 0 || delimiterp(*str) == t)
	    //If it's the end of the symbol, return the symbol object
	    return to_ret;
	  str++;
	}
      //Add another symbol object to the end of our symbol
      s->cdr = (pair*)kallocate();
      s = (symbol*)s->cdr;
      s->type = SYMBOL;
      s->cdr = nil;
    }
}

//Primitives

//Is it nil?
inline pair *knull (pair *exp)
{
  if (exp == nil ||
      (exp->type = LIST &&
       exp->car == nil &&
       exp->cdr == nil))
    return t;
  else
    return nil;
}

inline pair *klistp (pair *exp)
{
  if (exp->type == LIST)
    return t;
  else
    return nil;
}

inline pair *ksymbolp (pair *exp)
{
  if (exp->type == SYMBOL)
    return t;
  else
    return nil;
}

inline pair *kcar (pair *exp)
{
  if (klistp(exp) == t)
    return (pair*)exp->car;
  else
    return nil;//should raise an error
}

inline pair *kcdr (pair *exp)
{
  if (klistp(exp) == t)
    return (pair*)exp->cdr;
  else
    return nil; //should raise an error
}

inline pair *keq (pair *exp1, pair *exp2)
{
  if (exp1 == exp2)
    return t;
  else
    return nil;
}

inline pair *kequal (pair *exp1, pair *exp2)
{
  if ((exp1->type != exp2->type) && (knumberp(exp1) == nil))
    //If the types don't match, don't bother checking anything else, they're not equal.
    return nil;
  else if (keq(exp1, exp2) == t)
    //If they're the exact same object, they are equal.
    return t;
  else if (knumberp(exp1) == t)
    return nil;
  switch (exp1->type)
    {
    case(SYMBOL):
    case(STRING):
      while (1)
	{
	  int i;
	  string *s1 = (string*)exp1;
	  string *s2 = (string*)exp2;
	  for (i=0;i<8;i++)
	    //Compare the text in each string/symbol
	    {
	      if (s1->text[i] != s2->text[i])
		//If they don't match, nil.
		return nil;
	      else if (s1->text[i] == 0)
		//If it's the end of the string, t.
		return t;
	    }
	  //Otherwise, move to the next section of each symbol/string
	  exp1 = exp1->cdr;
	  exp2 = exp2->cdr;
	}
    case(LIST):
      while(1)
	{
	  if (kequal(exp1->car, exp2->car) == nil)
	    return nil;
	  else if (knull(exp1) == t)
	    return t;
	  exp1 = exp1->car;
	  exp2 = exp2->car;
	}
    default:
      return nil;
    }
}

inline pair *knumberp (pair *exp)
{
  switch (exp->type)
    {
    case(FLOATING):
    case(INTEGER):
      return t;
    default:
      return nil;
    }
}

inline pair *katom (pair *exp)
{
  if (klistp(exp) == t && knull(exp) == nil)
    return nil;
  else
    return t;
}

inline pair *kquote (pair *exp)
{
  return exp;
}

inline pair *knth (int i, pair *l)
{
  for (i;i>0;i--)
    {
      l = l->cdr;
    }
  return l;
}

//Meat and potatoes

char *skipwhite (char *str)
{
  while (*str == ' ' || *str == '\n' || *str == '\t')
    {
      str++;
      if (*str == '\0')
	return str;
    }
  if (*str == ';')
    {
      while (*str != '\n')
	{			
	  str++;
	  if (*str == '\0')
	    return str;
	}
    }
  return str;
}

pair *delimiterp(char c)
{
  if (whitespacep(c) == t || c == '(' || c == ')' || c == '\0' || c == '.' || c == ';')
    return t;
  else
    return nil;
}


/******************************************************
 *    kread() is very, very long.                     *
 *    Translates a string into a lisp expression.     *
 *****************************************************/
pair *kread (char **str)
{
  *str = skipwhite(*str); 
  if (**str == '(')//Read a list
    {
      *str = *str+1;
      *str = skipwhite(*str);
      //Allocate the space, and add a proper type tag
      pair *to_ret = kallocate();
      pair *p = to_ret;
      p->type - LIST;
      p->car = kread(str);//Read the car of the list
      p->cdr = nil;
      *str = skipwhite(*str);
      while (**str != ')' && **str != 0)//Read the rest of the list
	{
	  p->cdr = (pair*)kallocate();
	  p = p->cdr;
	  p->type = LIST;
	  p->car = (pair*)kread(str);
	  p->cdr = nil;
	  *str = skipwhite(*str);
	}
      return to_ret;	  
    }
  else if (**str >= '0' && **str <='9')//Read a number
    {
      long n = 0;
      while (**str >= '0' && **str <='9')//Read the portion before the decimal, or delimiter.
	{
	  n = (n * 10) + ((**str - '0') % 10);
	  *str = *str+1;
	}
      if (**str == '.')//If there's a decimal, this is a float; read the sub-zero numbers.
	{
	  *str = *str+1;
	  double i = 10;
	  floating *number = kallocate();
	  number->type = FLOATING;
	  number->num = n;
	  while (**str >= '0' && **str <='9')
	    {
	      number->num = number->num + (((**str - '0') % 10) / i);
	      i = i * 10;
	      *str = *str + 1;
	    }
	  return (pair*)number;
	}
      else//Otherwise, this is an integer: allocate space for it, and return it.
	{
	  integer *number = kallocate();
	  number->type = INTEGER;
	  number->num = n;
	  return (pair*)number;
	}
    }
  else if (**str == '\"')//Read a string
    {
      *str = *str+1;
      string *s = kallocate();
      string *to_ret = s;
      int i;

      s->type = STRING;
      s->cdr = nil;

      while (1)
	{
	  for (i=0;i<8;i++)
	    {
	      if (**str == '\\')
		{
		  *str = *str+1;
		  s->text[i] = **str;
		}
	      else if (**str == '\"')
		{
		  *str = *str + 1;
		  return (pair*)to_ret;
		}
	      else
		 s->text[i] = **str;
	      *str =  *str+1;
	    }
	  s->cdr = (pair*)kallocate();
	  s = (string*)s->cdr;
	  s->type = STRING;
	  s->cdr = nil;
	}
    }
  else//Read a symbol
    {
      symbol *s = kallocate();
      symbol *to_ret = s;
      int i;

      s->type = SYMBOL;
      s->cdr = nil;

      while (1)
	{
	  for (i=0;i<8;i++)
	    {
	      if (delimiterp(**str) == t)
		{
		  *str = *str + 1;
		  return (pair*)to_ret;
		}
	      else
		 s->text[i] = **str;
	      *str =  *str+1;
	    }
	  s->cdr = (pair*)kallocate();
	  s = (symbol*)s->cdr;
	  s->type = SYMBOL;
	  s->cdr = nil;
	}
    }
}

pair *keval (pair *exp, pair *env)
{
  if (katom(exp) == t)
    {
      if (knull(exp) == t)
	return nil;
      else if (exp->type == T)
	return t;
      else 
	return ob_assoc((symbol*)exp, env);//Change to assoc!
    }
  else if (katom(kcar(exp)) == t)
    {
      return nil;//Change to function name checks!
    }
  else
    return nil;
}

//Hash Table/oblist Layout:
//         (hashn1)---------->(hashn2)
//             | 
//            < --------->< >
//             |           |
//            < --><val>  < --><val
//             |           |
//           <key1>      <key2>

pair *new_oblist()
{
  int i;
  pair *to_ret = kallocate();
  pair *p = to_ret;
  p->type = LIST;
  p->car = nil;
  p->cdr = nil;
  for (i=0;i<255;i++)
    {
      p->cdr = kallocate();
      p = p->cdr;
      p->car = nil;
      p->cdr = nil;
    }
  return to_ret;
}

pair *ob_assoc(symbol *sym, pair *env)
{
  pair *oblist = kcar(env);
  int hash = (int)sym->text % 255;
  //creates a hash value with my awesomely high-tech hash function.

  pair *entry = knth(hash, oblist);
  pair *i = entry->car;
  while(1)
    {
      if (kequal((pair*)sym, (pair*)i->car->car) == t)
	return i;
      else if (i->cdr = nil)
	break;
      else 
	i = i->cdr;
    }
  //Add out new key to the oblist entry
  i = kallocate();
  i->cdr = entry->car;
  entry->car = i;

  //Link to the symbol
  i->car = kallocate();
  i->car->type = LIST;
  i->car->car = (pair*)sym;
  //Create the variable namespace
  i->car->cdr = kallocate();
  i->car->cdr->type = LIST;
  i->car->cdr->car = nil;
  //Create the function namespace
  i->car->cdr->cdr = kallocate();
  i->car->cdr->cdr->type = LIST;
  i->car->cdr->cdr->car = nil;
  i->car->cdr->cdr->cdr = nil;
  return i;
}  

int main ()
{
  init();
  pair *env;

  char **carname = malloc(4);
  carname = "car";
  symbol *s1 = strtosym(carname);
  symbol *s2 = strtosym(carname);

  char **str = malloc(15);
  str = "(+ (1.4 5))";
  pair *hope = (floating*)kread(&str); 
  if (kequal((pair*)s1, (pair*)s2) == nil)
    return 1;
  else
    return 0;
}
