/* lbind.c
 *
 * Definitions for Lisp interfaces to C functions. The C functions defined in 
 * lisp.c use C calling convention. These interfaces make use of the Lisp 
 * environment to find their parameters.
 *
 * This code is released under the GNU GPL General Public License.
 */

#include "lisp.h"
#include "lbind.h"

//Major TODO typechecking

/*Predicates*/
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

/*List operators*/
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

cons *llist(cons *env)
{
  cons *args = lookup("ARGS", env);
  cons *to_ret = newcons();
  cons *foo = to_ret;

  foo->car = args;
  args = args->cdr;

  for (args;args!=nil;args=args->cdr)
    {
      foo->cdr = newcons();
      foo = foo->cdr;
      foo->car = args;
    }
  return to_ret;  
}  

/*Mathematics*/
//TODO does not yet support bignums.
//TODO add math functions.

//Environment operators
symbol *lintern(cons *env)
{
  array *name = (array*)lookup("NAME", env);
  package *p = (package*)((procinfo*)env->car)->package_s->value;
  return intern(name, p);
}
  
package *lfind_package(cons *env)
{
  array *name = (array*)lookup("NAME", env);
  procinfo *p = (procinfo*)env->car;
  return find_package(name, p);
}
  
/*Equality*/


cons *lfind_class(cons *env)
{
  symbol *typename = (symbol*)lookup("TYPE-NAME", env);
  return typename->class;
}

cons *lchareq(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return chareq((base_char*)a, (base_char*)b);
}

cons *lcharequal(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return charequal((base_char*)a, (base_char*)b);
}

cons *lstringeq(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return stringeq((array*)a, (array*)b);
}

cons *lstringequal(cons *env)
{
  cons *a = lookup("A", env);
  cons *b = lookup("B", env);
  return stringequal((array*)a, (array*)b);
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

/*Flow control*/
cons *lcond(cons *env)
{
  cons *clauses = lookup("CLAUSES", env);
  cons *clause = clauses;
  while (1)
    {
      if (eval(clause->car->car, env) != nil)
	return eval(clause->car->cdr, env);
      else
	clause = clause->cdr;
    }
  return 0;//ERROR!
}

/*Evaluation*/
cons *leval(cons *env)
{
  cons *exp = lookup("EXP", env);
  return eval(exp, env);
}

cons *ltype_of(cons *env)
{
  cons *object = lookup("OBJECT", env);
  return type_of(object);
}

/*Assignment*/
cons *ldefun(cons *env)
{
  symbol *name = (symbol*)lookup("SYMBOL", env);
  cons *lambda_list = lookup("LAMBDA-LIST", env);
  cons *form = lookup("FORM", env);
  return (cons*)defun(name, lambda_list, form, env);
}

/*Special Operators*/
cons *lquote(cons *env)
{
  cons *exp = lookup("EXP", env);
  return exp;
}

//Reading
cons *lread_char(cons *env)
{
  stream *str = (stream*)lookup("STREAM", env);
  return (cons*)read_char(str);
}
