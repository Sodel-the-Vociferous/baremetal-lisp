#include <stdlib.h>
#include "lisp.h"

package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/*Keywords*/
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
//Constituent traits
symbol *invalid;
symbol *alphabetic;
symbol *alphadigit;
symbol *package_marker;
symbol *plus_sign;
symbol *minus_sign;
symbol *dot;
symbol *decimal_point;
symbol *ratio_marker;
//Types
symbol *ttype;
symbol *constype;
symbol *ttype;
symbol *constype;
symbol *fixnumtype;
symbol *bignumtype;
symbol *ratiotype;
symbol *singletype;
symbol *base_chartype;
symbol *vectortype;
symbol *arraytype;
symbol *compiled_functiontype;
symbol *stringtype;
symbol *symboltype;
symbol *functiontype;
symbol *hash_tabletype;
symbol *packagetype;
symbol *procinfotype;

/*Common-Lisp symbols*/
symbol *ts;//T symbol
symbol *nils;//NIL symbol
symbol *package_sym;//*package*
symbol *readtable;//*readtable*

//Lambda list control symbols
symbol *optional;//&optional
symbol *rest;//&rest
symbol *keyword;//&keyword
symbol *aux;//&aux
symbol *whole;//&whole
symbol *body;//&body
symbol *allow_other_keys;//&allow-other-keys



//This adorable little function saved me so much manual work. Global variables, here, are acceptable. :]
symbol *initsym(char *name, package *p)
{
  vector *a_name = strtolstr(name);
  symbol *a = fintern(a_name, p);
  if (p == keyword_pkg)
    {
      a->plist = fcons(mkpair((cons*)external, t), fcons(mkpair((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else if (p == cl_pkg)
    {
      a->plist = fcons(mkpair((cons*)external, t), nil);
    }
  return a;
}

symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env))
{
  symbol *funsym = fintern(strtolstr(name), p);
  funsym->plist = fcons(mkpair((cons*)external, t), fcons(mkpair((cons*)constant, t), nil));
  funsym->value = (cons*)newcompiled_function();
  compiled_function *f = (compiled_function*)funsym->value;
  f->plist = fcons(mkpair((cons*)external, t), fcons(mkpair((cons*)constant, t), nil));
  f->lambda_list = lambda_list;
  f->fun = fun;
  return funsym;
}

procinfo *init()
{
  long i;
  long j;

  //Init keyword package
  vector *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  //External and constant must be initialized manually, because they depend on themselves.
  external = fintern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = fintern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(mkpair((cons*)external, t), fcons(mkpair((cons*)constant, t), nil));
  constant->plist = fcons(mkpair((cons*)external, t), fcons(mkpair((cons*)constant, t), nil));

  //Other symbol attributes
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

  //Consituent traits
  invalid = initsym("INVALID", keyword_pkg);
  alphabetic = initsym("ALPHABETIC", keyword_pkg);
  alphadigit = initsym("ALPHADIGIT", keyword_pkg);
  package_marker = initsym("PACKAGE-MARKER", keyword_pkg);


  //Types
  ttype = initsym("T", keyword_pkg);
  constype = initsym("CONS", keyword_pkg);
  fixnumtype = initsym("FIXNUM", keyword_pkg);
  bignumtype = initsym("BIGNUM", keyword_pkg);
  ratiotype = initsym("RATIO", keyword_pkg);
  singletype = initsym("SINGLE", keyword_pkg);
  base_chartype = initsym("BASE-CHARACTER", keyword_pkg);
  vectortype = initsym("VECTOR", keyword_pkg);
  arraytype = initsym("ARRAY", keyword_pkg);
  compiled_functiontype = initsym("COMPILED-FUNCTION", keyword_pkg);
  stringtype = initsym("STRING", keyword_pkg);
  symboltype = initsym("SYMBOL", keyword_pkg);
  functiontype = initsym("FUNCTION", keyword_pkg);
  hash_tabletype = initsym("HASH-TABLE", keyword_pkg);
  packagetype = initsym("PACKAGE", keyword_pkg);
  procinfotype = initsym("PROCESS-INFO-TABLE", keyword_pkg);
  

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
      ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)constituent, t), nil);
  for (c=lowercase_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)constituent, t), nil);  
  for (c=whitespace_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)whitespace, t), nil);
  for (c=terminating_macro_chars;*c!=0;c++)
    ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)terminating_macro, nil), nil);//Change nil to macro function  
  for (c=non_terminating_macro_chars;*c!=0;c++)
    ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)non_terminating_macro, nil), nil);//Change nil to macro function
  for (c=single_escape_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)single_escape, t), nil);  
  for (c=multiple_escape_chars;*c!=0;c++)
      ((vector*)readtable_sym->value)->v[*c] =  fcons(mkpair((cons*)multiple_escape, t), nil);

  //Init lambda list control symbols
  optional = initsym("&OPTIONAL", cl_pkg);
  rest = initsym("&REST", cl_pkg);
  keyword = initsym("&KEYWORD", cl_pkg);
  aux = initsym("&AUX", cl_pkg);
  whole = initsym("&WHOLE", cl_pkg);
  body = initsym("&BODY", cl_pkg);
  allow_other_keys = initsym("&ALLOW-OTHER-KEYS", cl_pkg);

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
