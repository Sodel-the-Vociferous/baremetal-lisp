#include "lisp.h"
#include "lbind.h"

cons *lnull(cons *env)
{
  cons *a = lookup("A", env);
  return null(a);
}

cons *lnumberp(cons *env)
{
  cons *a = lookup("A", env);
  return numberp(a);
}

cons *lcons(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return fcons(a, b);
}
cons *lcar(cons *env)
{
  cons *a = lookup("A", env);
  return car(a);
}

cons *lrplaca(cons *env)
{
  cons *a = lookup("A", env);
  cons *new = lookup("NEW", env);
  return rplaca(a, new);
}

cons *lcdr(cons *env)
{
  cons *a = lookup("A", env);
  return cdr(a);
}

cons *lrplacd(cons *env)
{
  cons *a = lookup("A", env);
  cons *new = lookup("NEW", env);
  return rplacd(a, new);
}

symbol *lintern(cons *env)
{
  vector *name = (vector*)lookup("NAME", env);
  package *p = (package*)((procinfo*)env->car)->package_sym->value;
  return fintern(name, p);
}
  
package *lfind_package(cons *env)
{
  vector *name = (vector*)lookup("NAME", env);
  procinfo *p = (procinfo*)env->car;
  return find_package(name, p);
}
  
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

cons *leval(cons *env)
{
  cons *exp = lookup("EXP", env);
  return eval(exp, env);
}

cons *lassoc(cons *env)
{
  cons *key = lookup("KEY", env);
  cons *value = lookup("VALUE", env);
  return assoc(key, value);
}
