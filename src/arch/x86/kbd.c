#include "defs.h"
#include "io.h"
#include "interrupt.h"
#include "keymap.h"
#include "kbd.h"
#include "lisp.h"

extern uchar kbdus_noshift[];
extern uchar kbdus_shift[];

struct keyboard kbd;

uchar **keymap;
uchar **keymap_shift;

static void kbd_handler(struct registers);

static void kbd_handler(struct registers regs)
{
  char scancode;
  unsigned char c;
	
  /*read from keyboard data buffer*/
  scancode = inb(0x60);
	
  /*If the top bit is set, a key was just released*/
  if (scancode & 0x80)
    {
      scancode ^=  0x80;
      switch (c)
	{/* If the key (high bit stripped) is a status key, 
	  * set its status in the keyboard structure.
	  */
	case (LALT):
	  kbd.status.lalt = 0;
	  break;
	case (RALT):
	  kbd.status.ralt = 0;
	  break;
	case (LCTRL):
	  kbd.status.lctrl = 0;
	  break;
	case (RCTRL):
	  kbd.status.rctrl = 0;
	  break;
	case (LSHIFT):
	  kbd.status.lshift = 0;
	  break;
	case (RSHIFT):
	  kbd.status.rshift = 0;
	  break;
	  //Unfinished: implement remaining keys.
	default:
	  break;
	}
    }
	
  else //A key was just pressed.
    {
      write_char(ctolc(c), kbd.buffer);
    }
}

void init_kbd()
{
  register_interrupt_handler(IRQ1, &kbd_handler);
  keymap = (uchar**)&kbdus_noshift[0];
  keymap_shift = (uchar**)&kbdus_shift[0];  
}
