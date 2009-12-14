#include "defs.h"
#include "io.h"
#include "screen.h"
#include "lisp.c"

struct hw_screen screen;

void inithw_screen()
{
  screen->screen = (cchar*[])SCREENSTART;
}

void cursor(uint x, uint y)
{
  if ((x >= XCOLUMNS) ||
      (y >= YROWS))
    return;
  ushort location = y * ROWS + x; 

  outb(0x3D4, 14);
  outb(0x3D4, (uchar)location<<8);
  outb(0x3D4, 15);
  outb(0x3D5, (uchar)location);
}

void draw_screen(struct sw_screen *s)
{
  cursor(s->x, s->y);
  memcpy(&screen->screen[0], &s->screen[0], x * y * sizeof(cchar));
}

void advance_line(struct sw_screen *s)
{
  s->x = 0;
  s->y++;
  if (s->y >= YCHARS)
    {
      cchar *c;
      ushort y;
      for (y = 1, y < YROWS, y++)
	  memcpy(&s->screen[(y-1)*XCOLUMNS], &s->&s->screen[y*XCOLUMNS], XCOLUMNS);
      s->y--;
    }
}

void putchar(char c, struct sw_screen *s)
{ ushort location = y * YROWS + x; 
  struct cchar cc;
  cc->c = c;
  cc->fgattrib = s->fgattrib;
  cc->bgattrib = s->bgattrib;
  s->screen[location] = cc;
  s->x++;
  if (s->x >= XCOLUMNS)
    advance_line(s);
}
