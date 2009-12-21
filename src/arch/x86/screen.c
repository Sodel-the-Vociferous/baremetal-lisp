#include "defs.h"
#include "io.h"
#include "screen.h"
#include "lisp.c"

ushort *screen;

void cursor(uint x, uint y);
void cls(sw_screen *scn);
void draw_screen(struct sw_screen *s);
void newline(struct sw_screen *s);
void putchar(char c, struct sw_screen *scn);

void inithw_screen()
{
  screen = (ushort*)SCREENSTART;
  unsigned short blank = (' ' | (BLACK << 12) | (WHITE << 8));
  uint i;
  
  for (i=0; i<YROWS; i++)
    memsetw(screen + (i * XCOLUMNS), blank, XCOLUMNS);

  *screen = (ushort)('Q' | (BLACK << 12) | (LGREEN << 8));
  cursor(0, 0);
}

void cursor(uint x, uint y)
{
  ushort location = y * XCOLUMNS + x; 

  outb(0x3D4, 14);
  outb(0x3D5, location >> 8);
  outb(0x3D4, 15);
  outb(0x3D5, location);
}

void cls(sw_screen *scn)
{
  ushort blank = (' ' | (scn->bgattrib << 12) | (scn->fgattrib << 8));
  ushort *where = scn->screen;
  ushort *last = where + sizeof(ushort) * XCOLUMNS * YROWS;
  for(where; where < last; where = where + sizeof(ushort))
    *where = blank;
}

void newline(struct sw_screen *s)
{
  s->x = 0;
  s->y++;
  if (s->y >= YROWS)
    {
      s->y = s->y;//TODO!
    }
  cursor(s->x, s->y);
}

void putchar(char c, struct sw_screen *s)
{
  switch (c)
    {
    case('\n'):
      newline(s);
      return;
    case('\b'):
      return;
    default:
      if (c < ' ')
	return;
      break;
    }	  
  ushort *location = (ushort*)(s->y * XCOLUMNS + s->x + s->screen);
  *location = (c | (s->bgattrib << 12) | (s->fgattrib << 8));
  s->x++;
  if (s->x >= XCOLUMNS)
    newline(s);
}
