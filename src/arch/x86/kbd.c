#include "defs.h"
#include "io.h"
#include "interrupt.h"
#include "kbd.h"
#include "keymap.h"

struct keyboard kbd;
struct uchar **keymap;

static void kbd_handler(struct registers);

static void kbd_handler(struct registers regs)
{
  char scancode;
	
  /*read from keyboard data buffer*/
  scancode = inb(0x60);
	
  /*If the top bit is set, a key was just released*/
  if (scancode & 0x80)
    {
      switch (scancode ^ 0x80)
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
      if (kbd.widx = 0xff)//If we are about to wrap around...
	kbd.wrap = 1;//Tell the system so.
      kbd.buffer[kbd.widx] = scancode;
      kbd.widx++;
    }
}

void init_kbd()
{
  register_interrupt_handler(IRQ1, &kbd_handler);
}
