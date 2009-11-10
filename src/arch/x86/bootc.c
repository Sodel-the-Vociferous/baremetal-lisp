#include "defs.h"
#include "boot.h"
#include "io.h"
#include "interrupt.h"

/* Access to assembly_defined functions.
 */
extern void flush_gdt(uint);
extern void flush_idt(uint);

/* Internal function prototypes.
 */
static void init_gdt();
static void init_idt();
static void set_gdt_gate(int,uint,uint,uchar,uchar);
static void set_idt_gate(uchar,uint,ushort,uchar);

struct gdt_entry gdt_entries[5];
struct gdt_pointer gdt_ptr;
struct idt_entry idt_entries[256];
struct idt_pointer idt_ptr; 

static void init_dt()
{
  init_gdt();
  init_idt();
}

static void init_gdt()
{
  gdt_ptr.limit = (sizeof(struct gdt_entry) * 5) - 1;
  gdt_ptr.base  = (uint)&gdt_entries;

  set_gdt_gate(0, 0, 0, 0, 0);                // Null segment
  set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

  flush_gdt((uint)&gdt_ptr);
}

static void set_gdt_gate(int num, uint base, uint limit, uchar access, uchar granularity)
{
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_mid = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;
  
  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = (limit >> 16) & 0x0F;
  
  gdt_entries[num].granularity |= granularity & 0xF0;
  gdt_entries[num].access = access;
} 

static void init_idt()
{
    idt_ptr.limit = sizeof(struct idt_entry) * 256 -1;
    idt_ptr.base  = (uint)&idt_entries;

    memset((uchar*)&idt_entries, 0, sizeof(struct idt_entry)*256);

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

    set_idt_gate( 0, (unsigned)isr0 , 0x08, 0x8E);
    set_idt_gate( 1, (unsigned)isr1 , 0x08, 0x8E);
    set_idt_gate( 2, (unsigned)isr2 , 0x08, 0x8E);
    set_idt_gate( 3, (unsigned)isr3 , 0x08, 0x8E);
    set_idt_gate( 4, (unsigned)isr4 , 0x08, 0x8E);
    set_idt_gate( 5, (unsigned)isr5 , 0x08, 0x8E);
    set_idt_gate( 6, (unsigned)isr6 , 0x08, 0x8E);
    set_idt_gate( 7, (unsigned)isr7 , 0x08, 0x8E);
    set_idt_gate( 8, (unsigned)isr8 , 0x08, 0x8E);
    set_idt_gate( 9, (unsigned)isr9 , 0x08, 0x8E);
    set_idt_gate(10, (unsigned)isr10, 0x08, 0x8E);
    set_idt_gate(11, (unsigned)isr11, 0x08, 0x8E);
    set_idt_gate(12, (unsigned)isr12, 0x08, 0x8E);
    set_idt_gate(13, (unsigned)isr13, 0x08, 0x8E);
    set_idt_gate(14, (unsigned)isr14, 0x08, 0x8E);
    set_idt_gate(15, (unsigned)isr15, 0x08, 0x8E);
    set_idt_gate(16, (unsigned)isr16, 0x08, 0x8E);
    set_idt_gate(17, (unsigned)isr17, 0x08, 0x8E);
    set_idt_gate(18, (unsigned)isr18, 0x08, 0x8E);
    set_idt_gate(19, (unsigned)isr19, 0x08, 0x8E);
    set_idt_gate(20, (unsigned)isr20, 0x08, 0x8E);
    set_idt_gate(21, (unsigned)isr21, 0x08, 0x8E);
    set_idt_gate(22, (unsigned)isr22, 0x08, 0x8E);
    set_idt_gate(23, (unsigned)isr23, 0x08, 0x8E);
    set_idt_gate(24, (unsigned)isr24, 0x08, 0x8E);
    set_idt_gate(25, (unsigned)isr25, 0x08, 0x8E);
    set_idt_gate(26, (unsigned)isr26, 0x08, 0x8E);
    set_idt_gate(27, (unsigned)isr27, 0x08, 0x8E);
    set_idt_gate(28, (unsigned)isr28, 0x08, 0x8E);
    set_idt_gate(29, (unsigned)isr29, 0x08, 0x8E);
    set_idt_gate(30, (unsigned)isr30, 0x08, 0x8E);
    set_idt_gate(31, (unsigned)isr31, 0x08, 0x8E);
    set_idt_gate(32, (unsigned)irq0, 0x08, 0x8E);
    set_idt_gate(33, (unsigned)irq1, 0x08, 0x8E);
    set_idt_gate(34, (unsigned)irq2, 0x08, 0x8E);
    set_idt_gate(35, (unsigned)irq3, 0x08, 0x8E);
    set_idt_gate(36, (unsigned)irq4, 0x08, 0x8E);
    set_idt_gate(37, (unsigned)irq5, 0x08, 0x8E);
    set_idt_gate(38, (unsigned)irq6, 0x08, 0x8E);
    set_idt_gate(39, (unsigned)irq7, 0x08, 0x8E);
    set_idt_gate(40, (unsigned)irq8, 0x08, 0x8E);
    set_idt_gate(41, (unsigned)irq9, 0x08, 0x8E);
    set_idt_gate(42, (unsigned)irq10, 0x08, 0x8E);
    set_idt_gate(43, (unsigned)irq11, 0x08, 0x8E);
    set_idt_gate(44, (unsigned)irq12, 0x08, 0x8E);
    set_idt_gate(45, (unsigned)irq13, 0x08, 0x8E);
    set_idt_gate(46, (unsigned)irq14, 0x08, 0x8E);
    set_idt_gate(47, (unsigned)irq15, 0x08, 0x8E);
    flush_idt((uint)&idt_ptr);

    init_pit(50);
}

static void set_idt_gate(uchar num, uint base, ushort sel, uchar flags)
{
  idt_entries[num].base_low = base & 0xFFFF;
  idt_entries[num].base_high = (base >> 16) & 0xFFFF;
  
  idt_entries[num].sel = sel;
  idt_entries[num].zero = 0;
  idt_entries[num].flags = flags;
} 
