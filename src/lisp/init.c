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
symbol *dynamic;
symbol *constant;
//Readtable attributes
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;

//Common-Lisp symbols
symbol *ts;//T symbol
symbol *nils;//NIL symbol
symbol *package_sym;
symbol *readtable;

//This adorable little function saved me so much manual work. Global variables, here, are acceptable. :]
symbol *initsym(char *name, package *p)
{
  vector *a_name = strtolstr(name);
  symbol *a = fintern(a_name, p);
  if (p == keyword_pkg)
    {
      a->plist = fcons(assoc((cons*)external, t), fcons(assoc((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else if (p == cl_pkg)
    {
      a->plist = fcons(assoc((cons*)external, t), nil);
    }
  return a;
}

procinfo *init()
{
  long i;
  long j;

  //Init keyword package
  vector *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  external = fintern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = fintern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(assoc((cons*)external, t), fcons(assoc((cons*)constant, t), nil));
  constant->plist = fcons(assoc((cons*)external, t), fcons(assoc((cons*)constant, t), nil));

  internal = initsym("INTERNAL", keyword_pkg);
  inherited = initsym("INHERITED", keyword_pkg);
  dynamic = initsym("DYNAMIC", keyword_pkg);

  //Readtable character attributes
  constituent = initsym("CONSTITUENT", keyword_pkg);
  whitespace = initsym("WHITESPACE", keyword_pkg);
  terminating_macro = initsym("TERMINATING-MACRO-CHARACTER", keyword_pkg);
  non_terminating_macro = initsym("NON-TERMINATING-MACRO-CHARACTER", keyword_pkg);
  single_escape = initsym("SINGLE-ESCAPE", keyword_pkg);
  multiple_escape = initsym("MULTIPLE-ESCAPE", keyword_pkg);


  //Init cl package
  vector *cl_name = strtolstr("COMMON-LISP");
  cl_pkg = newpackage();
  cl_pkg->name = cl_name;

  //Init t
  t->type = T;
  ts = initsym("T", cl_pkg);
  ts->value = t;
  
  //Init nil
  //vector *nil_name = strtolstr("NIL");
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
  nils = initsym("NIL", cl_pkg);
  nils->value = nil;

  package_sym = initsym("*PACKAGE*", cl_pkg);
  package_sym->value = (cons*)cl_pkg;

  //Init *readtable*
  vector *readtable_name = strtolstr("*READTABLE*");
  symbol *readtable_sym = fintern(readtable_name, cl_pkg);
  readtable_sym->value = (cons*)newvector(255);
  char standard_chars[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ1234567890!$\"'(),_-./:;?+<=>#%&*@[\\]{|}`^~\b\t\n ";
  char uppercase_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char lowercase_chars[] = "abcdefghijklmnopqrstuvwxyz";
  char whitespace_chars[] = "\t\n";
  char terminating_macro_chars[] = "\"\'(),;`";
  char non_terminating_macro_chars[] = "#";
  char single_escape_chars[] = "\\";
  char multiple_escape_chars[] = "|";
  char *c;
  base_char *bc;
  
  //Create the default readtable
  for (c=uppercase_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)constituent, t), nil);
  for (c=lowercase_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)constituent, t), nil);  
  for (c=whitespace_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)whitespace, t), nil);
  for (c=terminating_macro_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)terminating_macro, t), nil);  
  for (c=non_terminating_macro_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)non_terminating_macro, t), nil);  
  for (c=single_escape_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)single_escape, t), nil);  
  for (c=multiple_escape_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(assoc((cons*)multiple_escape, t), nil);
 
  //Init cl-user package
  vector *cl_user_name = strtolstr("COMMON_LISP_USER");
  cl_user_pkg = newpackage();
  cl_user_pkg->name = cl_user_name;


  //init process info
  procinfo *main = malloc(sizeof(procinfo));
  main->type = PROCINFO;
  main->package_sym  = package_sym;
  main->packages = newcons();
  main->packages->car = (cons*)cl_pkg;
  main->packages->cdr = newcons();
  main->packages->cdr->car = (cons*)cl_user_pkg;
  main->packages->cdr->cdr = newcons();
  main->packages->cdr->cdr->car = (cons*)keyword_pkg;  

  return main;
}
