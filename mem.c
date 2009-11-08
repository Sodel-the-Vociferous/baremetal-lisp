#include "lisp.h"
#include "mem.h"
#include <stdlib.h>

//temporary
pair *newpair ()
{
  p = malloc(sizeof(pair));
  p.type = 0;
  p.car = nil;
  p.cdr = nil;
}
