#include "defs.h"
#include "main.h"
#include "multiboot.h"
#include "lisp.h"

int kmain(multiboot_header_t *mboot)
{
  stream *str;
  char c;
  int paren_levels = 0;
  

  while (1)
    {
      str = newstream();
      str->rv = newsimple_vector(240);
      str->wv = str->rv;

      while (1)
	{
	  c = peek_char(str)->c;
	  if (c == '(')
	    {
	      paren_levels++;
	      while (paren_levels > 0)
		{
		  c = peek_char(str)->c;
		  if (peek_char(str)->c == ')')
		    paren_levels--;
		}
	    }
	  else if (c == '\n')
	    break;
	}

    }
}

