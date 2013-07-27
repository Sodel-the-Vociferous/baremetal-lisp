
/* Alias the interrupt requests.
 */
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

struct registers
{
    uint ds;                  // Data segment selector
    uint edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint int_no, err_code;    // Interrupt number and error code (if applicable)
    uint eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
}__attribute__((packed));

/* Enables registration of callbacks for interrupts or IRQs.
 * For IRQs, to prevent confusion, use the #defines above as the
 * first parameter.
 */

typedef void (*isr_t)(struct registers);//A function pointer.
void register_interrupt_handler(uchar n, isr_t handler);
