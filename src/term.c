#include "defs.h"
#include "console.h"
#include "malloc.h"
#include "lisp.h"

struct terminal mainterm;

struct terminal *initterm()
{
  struct terminal *term = &mainterm;
  term->stdin = newstream();
  term->stdin.wv = newsimple_vector(128);
  term->stdin.rv = term->stdin.wv;
  return term;
}

