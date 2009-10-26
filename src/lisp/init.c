#include <stdlib.h>
#include "lisp.h"

procinfo *init()
{
  //Init keyword package
  vector *keyword_name = strtolstr("KEYWORD");
  package *keyword = newpackage();
  keyword->name = keyword_name;

  //Init :external
  vector *external_name = strtolstr("EXTERNAL");
  symbol *external = fintern(external_name, keyword);
  external->plist = newcons();
  external->plist->car = fcons((cons*)external, t);
  external->value = (cons*)external;

  //Init :internal
  vector *internal_name = strtolstr("INTERNAL");
  symbol *internal = fintern(internal_name, keyword);
  internal->plist = newcons();
  internal->plist->car = fcons((cons*)external, t);
  internal->value = (cons*)internal;

  //Init :inherited
  vector *inherited_name = strtolstr("INHERITED");
  symbol *inherited = fintern(inherited_name, keyword);
  inherited->plist = newcons();
  inherited->plist->car = fcons((cons*)external, t);
  inherited->value = (cons*)inherited;


  //Init cl package
  vector *cl_name = strtolstr("COMMON-LISP");
  package *cl = newpackage();
  cl->name = cl_name;
 
  //Init t
  t->type = T;
  vector *t_name = strtolstr("T");
  symbol *t_sym = fintern(t_name, cl);
  t_sym->value = t;
  symbol *test = fintern(t_name, cl);
  t_sym->plist = newcons();
  t_sym->plist->car = fcons((cons*)external, t);
  
  //Init nil
  vector *nil_name = strtolstr("NIL");
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
  symbol *nil_sym = fintern(nil_name, cl);
  nil_sym->value = nil;
  nil_sym->plist = newcons();
  nil_sym->plist->car = fcons((cons*)external, t);

  //Init *package*
  vector *package_sym_name = strtolstr("*PACKAGE*");
  symbol *package_sym = fintern(package_sym_name, cl);
  package_sym->value = (cons*)cl;
  package_sym->plist = newcons();
  package_sym->plist->car = fcons((cons*)external, t);

  //Init cl-user package
  vector *cl_user_name = strtolstr("COMMON_LISP_USER");
  package *cl_user = newpackage();
  cl_user->name = cl_user_name;


  //init process info
  procinfo *main = malloc(sizeof(procinfo));
  main->type = PROCINFO;
  main->package_sym  = package_sym;
  main->packages = newcons();
  main->packages->car = (cons*)cl;
  main->packages->cdr = newcons();
  main->packages->cdr->car = (cons*)cl_user;
  main->packages->cdr->cdr = newcons();
  main->packages->cdr->cdr->car = (cons*)keyword;  

  return main;
}
