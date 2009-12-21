#include "defs.h"
#include "boot.h"
#include "main.h"
#include "multiboot.h"
#include "lisp.h"
#include "screen.h"
#include "malloc.h"
#include "terminal.h"

lisp_terminal *current_terminal;
extern void end;

int kmain(multiboot_header_t *mboot)
{
  firstfree = (void*)&end;
  inithw_screen();
  init_dt();
  init_kbd();

  __asm__ __volatile__ ("sti"); //Allow interrupts

  //  lisp_main();
  lisp_terminal *term = newlisp_terminal();
  cursor(0, 0);
  termmain(term);
  for(;;);
}

