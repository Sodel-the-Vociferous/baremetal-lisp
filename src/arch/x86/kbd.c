#include "defs.h"
#include "io.h"
#include "interrupt.h"
#include "kbd.h"
#include "lisp.h"
#include "screen.h"
#include "terminal.h"
#include KEYMAP

struct keyboard kbd;

base_char *local_keymap[128];
base_char *local_keymap_shift[128];

static void kbd_handler(struct registers);

int special_key(uchar c, char press_or_release)
{
  switch (c)
    {/* If the key (high bit stripped) is a status key, 
      * set its status in the keyboard structure.
      */
    case (LALT):
      kbd.status.lalt = press_or_release;
      break;
    case (RALT):
      kbd.status.ralt = press_or_release;
      break;
    case (LCTRL):
      kbd.status.lctrl = press_or_release;
      break;
    case (RCTRL):
      kbd.status.rctrl = press_or_release;
      break;
    case (LSHIFT):
      kbd.status.lshift = press_or_release;
      break;
    case (RSHIFT):
      kbd.status.rshift = press_or_release;
      break;
      //Unfinished: implement remaining keys.
    default:
      return 0;
    }
  return 1;
}

static void kbd_handler(struct registers regs)
{
  char scancode;
  uchar c;
	
  /*read from keyboard data buffer*/
  scancode = inb(0x60);
	
  /*If the top bit is set, a key was just released*/
  if (scancode & 0x80)
    {
      scancode ^= 0x80;
      uchar c = keymap[scancode];
      special_key(c, 0);
    }
	
  else //A key was just pressed.
    {
      int i = 1;
      c = keymap[scancode];
      //putchar('!', current_terminal->screen);
      if (special_key(c, 1) > 0)
	return;
      if ((c > '~'))
	return;
      
      if ((kbd.status.rshift > 0) ||
	  (kbd.status.lshift > 0))
	write_char(local_keymap_shift[scancode], current_terminal->stdin);
      else 
	write_char(local_keymap[scancode], current_terminal->stdin);
    }
}

void init_kbd()
{
  int i;
  for (i=0; i<128; i++)
    local_keymap[i] = ctolc(keymap[i]);
  for (i=0; i<128; i++)
    local_keymap_shift[i] = ctolc(keymap_shift[i]);
  register_interrupt_handler(IRQ1, &kbd_handler);  
}
