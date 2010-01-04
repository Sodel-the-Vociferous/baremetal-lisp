#include "defs.h"
#include "lisp.h"
#include "malloc.h"
#include "screen.h"
#include "terminal.h"
#include "io.h"

int locusts = 0;

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

cons *termmain(lisp_terminal *term)
{
  procinfo *proc = init();
  cons *env = extend_env(nil);
  env->car = (cons*)proc;
  test(proc);
  cursor(term->screen->x, term->screen->y);
  int idx = term->stdin->write_index;
  int paren_levels = 0;
  base_char *c;
  while (1)
    {
      term->screen->fgattrib = GREEN;
      putchar('L', term->screen);
      putchar('I', term->screen);
      putchar('S', term->screen);
      putchar('P', term->screen);
      putchar('>', term->screen);
      putchar(' ', term->screen);
      cursor(term->screen->x, term->screen->y);
      term->screen->fgattrib = WHITE;

      stream *tolisp = newstream(255);

      while (1)
	{
	  if (idx == term->stdin->write_index)
	    continue;
	  while (1)
	    {
	      c = read_char(term->stdin);
	      if (c == (base_char*)nil)
		break;
	      write_char(c, tolisp);
	      putchar(c->c, term->screen);
	      cursor(term->screen->x, term->screen->y);
	      idx = term->stdin->write_index;
	      if (c->c == '(')
		paren_levels++;
	      else if (c->c == ')')
		paren_levels--; 
	      else if (c->c == '\n')
		break;
	    }
	  if ((paren_levels == 0) &&
	      (c->c == '\n'))
	    break;
	}
      cons *exp = read(tolisp, env);
      cons *value = eval(exp, env);
      
      stream *fromlisp = newstream(128);
      badprint(value, fromlisp);

      while (1)
	{
	  c = read_char(fromlisp);
	  if (c == (base_char*)nil)
	    break;
	  putchar(c->c, term->screen);
	  cursor(term->screen->x, term->screen->y);
	}
      putchar('\n', term->screen);
      putchar('\n', term->screen);
    }
  for(;;);
}
 
