#include "defs.h"
#include "lisp.h"
#include "malloc.h"
#include "screen.h"
#include "terminal.h"
#include "io.h"
#include KEYMAP

int locusts = 0;

lisp_terminal *newlisp_terminal()
{
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
  
  //test(proc);
  
  cursor(term->screen->x, term->screen->y);
  
  /* Save the write index, so we know when stdin has changed. A kludge. */
  int idx = term->stdin->write_index;
  int paren_levels = 0;
  base_char *c;
  while (1)
    {
      term->screen->fgattrib = GREEN;
      /* It doesn't get much classier than this. */
      putchar('L', term->screen);
      putchar('I', term->screen);
      putchar('S', term->screen);
      putchar('P', term->screen);
      putchar('>', term->screen);
      putchar(' ', term->screen);
      cursor(term->screen->x, term->screen->y);
      term->screen->fgattrib = WHITE;
      
      /* Create a new stream to be sent as input to the Lisp interpreter. */
      stream *tolisp = newstream(255);
      
      while (1)
	{
	  if (idx == term->stdin->write_index)
	    /* If there has been no input, skip the rest of this loop iteration.
	     */
	    continue;

	  while (1)
	    {
	      c = read_char(term->stdin);
	      if (c == (base_char*)nil)
		/* If there is no more input from the stream, stop reading 
		 * from it.
		 */
		break;
	
	      if (c->c >= '~')
		break;
	      
	      if (c->c == ESCKEY)
		{
		  cursor(term->screen->x, term->screen->y);
		  paren_levels = 0;
		  break;
		}
	      
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
	      (c->c == '\n') ||
	      (c->c == ESCKEY))
	    {
	      idx = term->stdin->write_index;
	      break;
	    }
	}
      
      if (c->c != ESCKEY)
	{
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
	}
      
      putchar('\n', term->screen);
      putchar('\n', term->screen);
    }
  for(;;);
}
 
