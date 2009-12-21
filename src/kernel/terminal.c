#include "defs.h"
#include "lisp.h"
#include "malloc.h"
#include "screen.h"
#include "terminal.h"
#include "io.h"

lisp_terminal *newlisp_terminal()
{
  int i;
  lisp_terminal *term = malloc(sizeof(lisp_terminal));
  current_terminal = term;
  term->stdin = newstream(255);
  term->stdout = newstream(255);
  term->screen = malloc(sizeof(sw_screen));
  
  term->screen->fgattrib = WHITE;
  term->screen->bgattrib = BLACK;
  term->screen->x = 0;
  term->screen->y = 0;
  term->screen->screen = (ushort*)SCREENSTART;
  cls(term->screen);
  return term;
}

base_char *termread_char(stream *str)
{
  base_char *c;
  do 
    {
      c = read_char(str);
    } while (c == (base_char*)nil);
  return c;
}

cons *termmain(lisp_terminal *term)
{
  procinfo *proc = init();
  cons *env = extend_env(nil);
  env->car = (cons*)proc;
  test(proc);
  putchar('L', term->screen);
  putchar('I', term->screen);
  putchar('S', term->screen);
  putchar('P', term->screen);
  cursor(term->screen->x, term->screen->y);
  while (1)
    {
      base_char *c = termread_char(term->stdin);
      putchar(c->c, term->screen);
      cursor(term->screen->x, term->screen->y);
    }
}
  
