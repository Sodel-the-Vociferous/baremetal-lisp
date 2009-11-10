#include "defs.h"
#include "boot.h"
#include "io.h"
#include "interrupt.h"

isr interrupt_handlers[256];
uint tick = 0;

void isr_handler(struct registers regs)
{
  if (interrupt_handlers[regs.int_no] != 0)
    {
      isr handler = interrupt_handlers[regs.int_no];
      handler(regs);
    }
/*     else */
/*     { */
/*        	puts("unhandled interrupt: "); */
/*         putint(regs.int_no); */
/*         putc('\n'); */
/*     } */
}

void irq_handler(struct registers regs)
{/* Send an End-Of-Interupt signal to PICs.
  */
  if (regs.int_no > MASTER_MAX)
    /* If the slave PIC was involved, send it a reset signal
     * as well.
     */
    outb(0xA0, 0x20);
  
  outb(0x20, 0x20); /* Send reset signal to master, in any case. */
  
  if (interrupt_handlers[regs.int_no] != 0)
    {
      isr handler = interrupt_handlers[regs.int_no];
      handler(regs);
    }
}


void register_interrupt_handler(uchar n, isr handler)
{
  interrupt_handlers[n] = handler;
} 

//PIT stuff
static void timer_callback(struct registers regs)
{
  tick++;
}

void init_pit(uint frequency)
{
  // Firstly, register our timer callback.
  register_interrupt_handler(IRQ0, &timer_callback);
  
  // The value we send to the PIT is the value to divide it's input clock
  // (1193180 Hz) by, to get our required frequency. Important to note is
  // that the divisor must be small enough to fit into 16-bits.
  uint divisor = 1193180 / frequency;
  
  // Send the command byte.
  outb(0x43, 0x36);

  // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
  uchar l = (uchar)(divisor & 0xFF);
  uchar h = (uchar)( (divisor>>8) & 0xFF );
  
  // Send the frequency divisor.
  outb(0x40, l);
  outb(0x40, h);
} 
