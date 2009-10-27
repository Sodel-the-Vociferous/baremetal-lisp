#include <stdlib.h>
#include "lisp.h"

package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

//Keywords
//Symbol attributes
symbol *internal;
symbol *external;
symbol *inherited;
//Readtable attributes
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;

//Common-Lisp symbols
symbol *package;
symbol *readtable;



procinfo *init()
{
  long i;
  long j;


  //Init keyword package
  vector *keyword_name = strtolstr("KEYWORD");
  package *keyword = newpackage();
  keyword->name = keyword_name;

  //Init :external
  vector *external_name = strtolstr("EXTERNAL");
  symbol *external = fintern(external_name, keyword);
  external->plist = fcons(assoc((cons*)external, t), nil);
  external->value = (cons*)external;

  //Init :internal
  vector *internal_name = strtolstr("INTERNAL");
  symbol *internal = fintern(internal_name, keyword);
  internal->plist = fcons(assoc((cons*)external, t), nil);
  internal->value = (cons*)internal;

  //Init :inherited
  vector *inherited_name = strtolstr("INHERITED");
  symbol *inherited = fintern(inherited_name, keyword);
  inherited->plist = fcons(assoc((cons*)external, t), nil);
  inherited->value = (cons*)inherited;

  //Init :constituent
  vector *constituent_name = strtolstr("CONSTITUENT");
  symbol *constituent = fintern(constituent_name, keyword);
  constituent->plist = fcons(assoc((cons*)external, t), nil);
  constituent->value = (cons*)constituent;

  //Init :whitespace
  vector *whitespace_name = strtolstr("WHITESPACE");
  symbol *whitespace = fintern(whitespace_name, keyword);
  whitespace->plist = fcons(assoc((cons*)external, t), nil);
  whitespace->value = (cons*)whitespace;

  //Init :terminating-macro
  vector *terminating_macro_name = strtolstr("TERMINATING-MACRO");
  symbol *terminating_macro = fintern(terminating_macro_name, keyword);
  terminating_macro->plist = fcons(assoc((cons*)external, t), nil);
  terminating_macro->value = (cons*)terminating_macro;

  //Init :non-terminating-macro
  vector *non_terminating_macro_name = strtolstr("NON_TERMINATING_MACRO");
  symbol *non_terminating_macro = fintern(non_terminating_macro_name, keyword);
  non_terminating_macro->plist = fcons(assoc((cons*)external, t), nil);
  non_terminating_macro->value = (cons*)non_terminating_macro;

  //Init :single-escape
  vector *single_escape_name = strtolstr("SINGLE-ESCAPE");
  symbol *single_escape = fintern(single_escape_name, keyword);
  single_escape->plist = fcons(assoc((cons*)external, t), nil);
  single_escape->value = (cons*)single_escape;

  //Init :multiple-escape
  vector *multiple_escape_name = strtolstr("MULTIPLE-ESCAPE");
  symbol *multiple_escape = fintern(multiple_escape_name, keyword);
  multiple_escape->plist = fcons(assoc((cons*)external, t), nil);
  multiple_escape->value = (cons*)multiple_escape; 


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
  t_sym->plist = fcons(assoc((cons*)external, t), nil);
  
  //Init nil
  vector *nil_name = strtolstr("NIL");
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
  symbol *nil_sym = fintern(nil_name, cl);
  nil_sym->value = nil;
  nil_sym->plist = fcons(assoc((cons*)external, t), nil);

  //Init *package*
  vector *package_sym_name = strtolstr("*PACKAGE*");
  symbol *package_sym = fintern(package_sym_name, cl);
  package_sym->value = (cons*)cl;
  package_sym->plist = fcons(assoc((cons*)external, t), nil);

  //Init *readtable*
  vector *readtable_name = strtolstr("*READTABLE*");
  symbol *readtable_sym = fintern(readtable_name, cl);
  readtable_sym->value = (cons*)newvector(255);
  char standard_chars[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ1234567890!$\"'(),_-./:;?+<=>#%&*@[\\]{|}`^~\b\t\n ";
  char uppercase_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char lowercase_chars[] = "abcdefghijklmnopqrstuvwxyz";
  char whitespace_chars[] = "\t\n";
  char terminating_macro_chars[] = "\"\'(),;`";
  char non_terminating_macro_chars[] = "#";
  char single_escape_chars[] = "\\";
  char mutple_escape_chars[] = "|";
  
  base_char *c;
  vector *readtable_vector = ((vector*)readtable_sym->value);
  
  readtable_sym->plist = fcons(assoc((cons*)external, t), nil);


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
