;;; Assembly infrastructure for Global Descriptor Table, and
;;; Interrupt descriptor table.

	[GLOBAL gdt_flush]		;Allow C code to flush GDT.
	[GLOBAL idt_flush]		;Allow C code to flush IDT.

gdt_flush:
	mov eax, [esp+4]  		; Get the pointer to the GDT, passed as a parameter.
	lgdt [eax]			; Load the new GDT pointer

	mov ax, 0x10      		; 0x10 is the offset in the GDT to our data segment
	mov ds, ax        		; Load all data segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush   		; 0x08 is the offset to our code segment: Far jump!
					; I don't actually know why this is required.
.flush:
   ret

idt_flush:
	mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter.
	lidt [eax]        ; Load the IDT pointer.
	ret

	