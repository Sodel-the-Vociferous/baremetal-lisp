/* dt.c
 *
 * Initializes the Interrupt Descriptor Table and Global Descriptor
 * Table, and defines the default handler for Interrupt Requests, and
 * Interrupt Service Routines.
 */

#include "defs.h"
#include "dt.h"

// Defined in dtasm.s
extern void gdt_flush(uint);
extern void idt_flush(uint);

// Internal function prototypes.
static void init_gdt();
static void gdt_set_gate(int,uint,uint,uchar,uchar);
static void init_idt();
static void idt_set_gate(uchar,uint,ushort,uchar);

void init_dt()
{/* Initializes the descriptor tables.
  */
  init_gdt();
  init_idt();
  __asm__ __volatile__ ("sti"); //Allow interrupts
}

static void init_gdt()
{/* Initializes the initial Global Descriptor Table.
  */
  gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
  gdt_ptr.base  = (uint)&gdt_entries;
  
  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
  
  gdt_flush((uint)&gdt_ptr);
}

static void gdt_set_gate(sint num, uint base, uint limit, uchar access, uchar gran)
{/* Sets the value of one GDT entry.
  */
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_middle = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;
  
  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = (limit >> 16) & 0x0F;
  
  gdt_entries[num].granularity |= gran & 0xF0;
  gdt_entries[num].access = access;
} 

static void init_idt()
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
    idt_ptr.base  = (uint)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

    // Remap the irq table.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate( 0, (unsigned)isr0 , 0x08, 0x8E);
    idt_set_gate( 1, (unsigned)isr1 , 0x08, 0x8E);
    idt_set_gate( 2, (unsigned)isr2 , 0x08, 0x8E);
    idt_set_gate( 3, (unsigned)isr3 , 0x08, 0x8E);
    idt_set_gate( 4, (unsigned)isr4 , 0x08, 0x8E);
    idt_set_gate( 5, (unsigned)isr5 , 0x08, 0x8E);
    idt_set_gate( 6, (unsigned)isr6 , 0x08, 0x8E);
    idt_set_gate( 7, (unsigned)isr7 , 0x08, 0x8E);
    idt_set_gate( 8, (unsigned)isr8 , 0x08, 0x8E);
    idt_set_gate( 9, (unsigned)isr9 , 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
    idt_flush((uint)&idt_ptr);
}

static void idt_set_gate(uchar num, uint base, ushort sel, uchar flags)
{
  idt_entries[num].base_lo = base & 0xFFFF;
  idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
  
  idt_entries[num].sel     = sel;
  idt_entries[num].always0 = 0;
  idt_entries[num].flags   = flags 
} 
