/* lbind.c
 *
 * Daniel Ralston, 2009
 *
 * Definitions for Lisp interfaces to C functions. The C functions defined in 
 * lisp.c use C calling convention. These interfaces make use of the Lisp 
 * environment to find their parameters. 
 *
 * This code is released under the GNU GPL General Public License.
 */

#include "lisp.h"
#include "init.h"
#include "lbind.h"

cons *llet_base(cons *env, int parallelp);

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
  cons *a = eval((cons*)a_s, env);
  cons *b = eval((cons*)b_s, env);
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
  cons *args = eval((cons*)args_s, env);
  cons *arg = args;
  cons *to_ret = newcons();
  cons *foo = to_ret;

  if (args == nil)
    return nil;
  foo->car = arg->car;
  arg = arg->cdr;

  for (arg;arg!=nil;arg=arg->cdr)
    {
      foo->cdr = newcons();
      foo = foo->cdr;
      foo->car = arg->car;
    }
  return to_ret;  
}  

/*Mathematics*/
//TODO support bignums.
//TODO only quick-and-dirty
//TODO division
cons *ladd(cons *env)
{
  cons *args = eval((cons*)args_s, env);
  cons *foo;

  fixnum *num = newfixnum(0);
  for(foo=args; foo!=nil; foo=foo->cdr)
    num->num += ((fixnum*)foo->car)->num;
  return (cons*)num;
}

cons *lsubtract(cons *env)
{
  cons *args = eval((cons*)args_s, env);
  cons *foo;

  fixnum *num;

  if (args->cdr == nil)
    {
      num = newfixnum(0);
      foo = args;
    }
  else
    {
      fixnum *first = (fixnum*)args->car;
      num = newfixnum(first->num);
      foo = args->cdr;
    }
  
  for(foo; foo!=nil; foo=foo->cdr)
    num->num -= ((fixnum*)foo->car)->num;
  return (cons*)num;
}

cons *lmultiply(cons *env)
{
  cons *args = eval((cons*)args_s, env);
  cons *foo;

  fixnum *num = newfixnum(1);
  for(foo=args; foo!=nil; foo=foo->cdr)
    num->num *= ((fixnum*)foo->car)->num;
  return (cons*)num;
}

cons *lnum_eq(cons *env)
{
  fixnum *a = (fixnum*)lookup("A", env);
  fixnum *b = (fixnum*)lookup("B", env);
  
  if (a->num == b->num)
    return t;
  else
    return nil;
}

/* Environment */
cons *lintern(cons *env)
{
  array *name = (array*)lookup("NAME", env);
  package *p = (package*)((procinfo*)env->car)->package_s->value;
  return (cons*)intern(name, p);
}
  
cons *lfind_package(cons *env)
{
  array *name = (array*)lookup("NAME", env);
  procinfo *p = (procinfo*)env->car;
  return (cons*)find_package(name, p);
}

cons *lfind_class(cons *env)
{
  symbol *typename = (symbol*)lookup("TYPE-NAME", env);
  return typename->class;
}
  
/*Equality*/
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

/* cons *lhash (cons *env) */
/* { */
/*   cons *object = eval((cons*)object_s,env); */
/*   hash_table *ht = (hash_table*)eval((cons*)hash_table_s, env); */
  
/*   return (cons*)newfixnum(hash(object, ht)); */
/* } */

/*Flow control*/
cons *lcond(cons *env)
{
  cons *clauses = lookup("CLAUSES", env);
  cons *clause = clauses;
  while (1)
    {
      if (clause == nil)
	return 0;//TODO error
      else if (eval(clause->car->car, env) != nil)
	return eval(clause->car->cdr->car, env);
      else
	clause = clause->cdr;
    }
  return 0;//TODO ERROR!
}

/*Evaluation*/
cons *lquote(cons *env)
{
  cons *exp = eval((cons*)exp_s, env);
  return exp;
}
cons *leval(cons *env)
{
  cons *exp = eval((cons*)exp_s, env);
  return eval(exp, env);
}

cons *lprogn(cons *env)
{
  cons *body = lookup("BODY", env);
  cons *to_ret;
  for(body;body!=nil;body=body->cdr)
    to_ret = eval(body->car, env);
  return to_ret;
}

cons *lfunction(cons *env)
{
  function *f = (function*)lookup("NAME", env);
  if(typep((cons*)f, function_s) == t)
    return f->fun;
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

  cons *proper_env = env->cdr;
  for(proper_env; proper_env->cdr!=nil; proper_env=proper_env->cdr);
  proper_env = proper_env->car;

  return (cons*)defun(name, lambda_list, form, proper_env);
}

cons *llet_base(cons *env, int parallelp)
{//TODO declarations
  cons *oldenv = env;
  cons *body = lookup("BODY", env);
  symbol *sym;
  cons *value;
  cons *varforms = body->car;
  cons *evalforms = body->cdr;
  cons *form;
  cons *to_ret;
  cons *old_dbindings = nil;

  if (parallelp == 1)
    /* If variables are to be assigned in parallel (let...), separate the old 
     * and new environments. If the bindings are sequential (let*...), no such 
     * separation is needed, as a variable's initial value can refer to 
     * bindings previously made during this "instance" of (let*...).
     */
    env = extend_env(env);

  for(form=varforms; form!=nil; form=form->cdr)
    {/* For each symbol or symbol-value pair:
      */
      if (typep(form->car, symbol_s) == t)
	{/* If the form is a symbol, bind it to nil.
	  */
	  sym = (symbol*)form->car;
	  value = nil;
	}
      else if ((typep(form->car, list_s) == t) &&
	       (typep(form->car->car, symbol_s) == t))
	{/* If the form is a symbol-value pair (that is, a two-item list: first
	  * the symbol, then the value), bind the symbol to the value.
	  */
	  sym = (symbol*)form->car->car;
	  value = eval(form->car->cdr->car, oldenv);
	}
      else
	/* If it is neither of these, throw an error.
	 */
	return 0;//TODO error

      if (assoc((cons*)special, sym->plist)->cdr != nil)
	{/* If the variable is special, save its current dynamic binding, and
	  * set the new dynamic value.
	  */
	  old_dbindings = fcons(fcons((cons*)sym, sym->value), 
				old_dbindings);
	  sym->value = value;
	}
      else
	/* Otherwise, bind the variable lexically.
	 */
	envbind(sym, value, env);
    }

  for(form=evalforms; form!=nil; form=form->cdr)
    /* Evaluate each constituent form similarly to progn, and store the value of
     * the final evaluation.
     */
    to_ret = eval(form->car, env);
  
  for(form=old_dbindings; form!=nil; form=form->cdr)
    {/* Restore any old dynamic bindings.
      */
      sym = (symbol*)form->car->car;
      value = form->car->cdr;

      sym->value = value;
    }

  return to_ret;
}

cons *llet(cons *env)
{
  return llet_base(env, 1);
}

cons *llet_star(cons *env)
{
  return llet_base(env, 0);
}

cons *lsetq(cons *env)
{
  symbol *s = (symbol*)eval((cons*)symbol_s, env);
  cons *value = eval((cons*)value_s, env);
  value = eval(value, env);
  cons *binding = env->cdr; /* current environment node */

  while (binding!=nil)
    {/* Loop through the lexical environment, searching for a binding for 
      * symbol. 
      */
      if ((symbol*)binding->car->car == s)
	{/* If the current binding's symbol is the one we're looking for, 
	  * set and return the value.
	  */
	  binding->car->cdr->car = value;
	  break;
	}
      else if (binding->cdr == nil)
	/* If we've run out of bindings at this "level" of the environment, 
	 * move to the next one.
	 */
	binding = binding->car->cdr;
      else
	/* If there are still bindings to search, go to the next one.
	 */
	binding = binding->cdr;
    }
  if (binding == nil)
    s->value = value;

  return value;
}

cons *ldefparameter(cons *env)
{
  symbol *sym = (symbol*)eval((cons*)symbol_s, env);
  cons *value = eval((cons*)value_s, env);
  value = eval(value, env);
  cons *docstring = lookup("DOCUMENTATION", env);
  cons *doc_p = lookup("DOCUMENTATION-P", env);

  return (cons*)defparameter(sym, value, docstring, doc_p);
}

cons *ldefvar(cons *env)
{
  symbol *sym = (symbol*)eval((cons*)symbol_s, env);
  cons *value = lookup("VALUE", env);
  cons *value_p = lookup("VALUE-P", env);
  if (value_p != nil)
    value = eval(value, env);
  cons *docstring = lookup("DOCUMENTATION", env);
  cons *doc_p = lookup("DOCUMENTATION-P", env);

  return (cons*)defvar(sym, value, value_p, docstring, doc_p);
}

/* Reading */
cons *lread_char(cons *env)
{
  stream *str = (stream*)lookup("STREAM", env);
  return (cons*)read_char(str);
}

cons *lread_list(cons *env)
{
  stream *str = (stream*)eval((cons*)list_s, env);
  base_char *c = (base_char*)eval((cons*)character_s, env);
  return read_list(str, c, env);
}
