#include "lisp.h"
#include "lbind.h"

//Major todo typechecking

//Predicates
cons *lnull(cons *env)
{
  cons *a = lookup("OBJECT", env);
  return null(a);
}

cons *lnumberp(cons *env)
{
  cons *a = lookup("OBJECT", env);
  return numberp(a);
}

//List operators
cons *lcons(cons *env)
{
  cons *a = lookup("OB1", env);
  cons *b = lookup("OB2", env);
  return fcons(a, b);
}
cons *lcar(cons *env)
{
  cons *a = lookup("LIST", env);
  return car(a);
}

cons *lrplaca(cons *env)
{
  cons *a = lookup("LIST", env);
  cons *new = lookup("NEWCAR", env);
  return rplaca(a, new);
}

cons *lcdr(cons *env)
{
  cons *a = lookup("LIST", env);
  return cdr(a);
}

cons *lrplacd(cons *env)
{
  cons *a = lookup("LIST", env);
  cons *new = lookup("NEWCDR", env);
  return rplacd(a, new);
}

//Mathematics
//TODO does not yet support bignums.
//TODO add math functions.

//Environment operators
symbol *lintern(cons *env)
{
  simple_vector *name = (simple_vector*)lookup("NAME", env);
  package *p = (package*)((procinfo*)env->car)->package_sym->value;
  return intern(name, p);
}
  
package *lfind_package(cons *env)
{
  simple_vector *name = (simple_vector*)lookup("NAME", env);
  procinfo *p = (procinfo*)env->car;
  return find_package(name, p);
}
  
//Equality
cons *leq (cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return eq(a, b);
}

cons *leql (cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return eql(a, b);
}

cons *lequal (cons *env);

//Evaluation
cons *lquote(cons *env)
{
  cons *exp = lookup("EXP", env);
  return exp;
}

cons *leval(cons *env)
{
  cons *exp = lookup("EXP", env);
  return eval(exp, env);
}

//Reading

