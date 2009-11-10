;;; boot.s
;;; Kernel begins execution here
;;; Uses nasm specific directives.

	[BITS 32]		;Use 32 bit instructions.
	
	[GLOBAL mboot]		;Make mboot accessible from C.
	[EXTERN code]		;Start of .text section.
	[EXTERN bss]		;Start of .bss section.
	[EXTERN end]		;End of the kernel.
	
	[GLOBAL start]		;Kernel entry point
	[EXTERN kmain]		;Entry to C code.
	[GLOBAL flush_gdt]
	[GLOBAL flush_idt]
	
	
	MBOOT_PAGE_ALIGN	equ	1<<0 ;Load kernel and any modules on a page boundary.
	MBOOT_MEM_INFO		equ	1<<1 ;Provide the kernel with memory information.
	MBOOT_HEADER_MAGIC	equ	0xBADB002 ;Multiboot magic number.
	MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
	MBOOT_CHECKSUM		equ 	-(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)
	STACKSIZE equ 0x4000   		;16Kib.

section .text
	align 4
	
mboot:				;Embed multiboot headers in the code, for GRUB to find.
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS	;How GRUB should load the kernel
	dd MBOOT_CHECKSUM	;Ensure the preceding aren't borked.

	dd mboot		;Make public the mboot header's location
	dd code			;Make public the .text section
	dd bss			;End of .text
	dd end			;End of kernel
	dd start		;Kernel entry 

start:
	push ebx 		;Load location of multiboot header to pass as a parameter to kmain.

	cli			;Disable interrupts
	call kmain

infinite_loop:	
	jmp infinite_loop	;Enter an infinite loop if kmain ever returns.

	section .bss
	align 32
stack:
	resb STACKSIZE		;Reserve 16K stack

flush_gdt:
	mov eax, [esp+4]  	;Get the pointer to the GDT, passed as a parameter.
	lgdt [eax]      	;Load the new GDT pointer

	mov ax, 0x10      	;0x10 is the offset in the GDT to our data segment
	mov ds, ax        	;Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush   	;0x08 is the offset to code segment: Far jump! 
.flush:
   ret

	[GLOBAL flush_idt]
flush_idt:
	mov eax, [esp+4]  	;Get the pointer to the IDT, passed as a parameter.
	lidt [eax]        	;Load the IDT pointer.
	ret

;;; Interrupts

	;; Define an ISR with no error code.
	%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
	[GLOBAL isr%1]        ; %1 accesses the first parameter.
isr%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common
	%endmacro

	;; Define an ISR with an error code.
	%macro ISR_ERRCODE 1
	[GLOBAL isr%1]
isr%1:
	cli
	push byte %1
	jmp isr_common
	%endmacro
	
;ISRs
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_NOERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

[EXTERN isr_handler]		;Defined in boot.c

;;; Common ISR stub. Saves processor state, sets up the processor
;;; for kernel-mode segment, and calls the C fault handler.
;;; When the handler returns, restores the stack frame.
isr_common:
   pusha                    	; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               	; Lower 16-bits of eax = ds.
   push eax                 	; save the data segment descriptor

   mov ax, 0x10  		; load the kernel data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call isr_handler

   pop eax        		; reload the original data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   popa                     	; Pops edi,esi,ebp...
   add esp, 8     		; Cleans up the pushed error code and pushed ISR number
   sti
   iret           		; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
   
%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common
%endmacro
   
;IRQs
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

[EXTERN irq_handler]		;Defined in boot.c

;;; Common IRQ stub. Saves processor state, sets up the processor
;;; for kernel-mode segment, and calls the C fault handler.
;;; When the handler returns, restores the stack frame.
irq_common:
   pusha                    	; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

   mov ax, ds               	; Lower 16-bits of eax = ds.
   push eax                 	; save the data segment descriptor

   mov ax, 0x10  		; load the kernel data segment descriptor
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call irq_handler

   pop ebx        		; reload the original data segment descriptor
   mov ds, bx
   mov es, bx
   mov fs, bx
   mov gs, bx

   popa                     	; Pops edi,esi,ebp...
   add esp, 8     		; Cleans up the pushed error code and pushed ISR number
   sti
   iret           		; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP 