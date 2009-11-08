	;; boot.s
	;; Kernel begins here

	MBOOT_PAGE_ALIGN	equ	1<<0 ;Load kernel and any modules on a page boundary.
	MBOOT_MEM_INFO		equ	1<<1 ;Provide the kernel with memory information.
	MBOOT_HEADER_MAGIC	equ	0xBADB002 ;Multiboot magic number.
	MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
	MBOOT_CHECKSUM		equ 	-(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

	[BITS 32]		;32 bit instructions.

	[GLOBAL mboot]		;Make mboot accessible from C.
	[EXTERN code]		;Start of .text section.
	[EXTERN bss]		;Start of .bss section.
	[EXTERN end]		;End of the kernel.

mboot:				;Embed multiboot headers in the code, for GRUB to find.
	dd MBOOT_HEADER_MAGIC
	dd MBOOT_HEADER_FLAGS	;How GRUB should load the kernel
	dd MBOOT_CHECKSUM	;Ensure the preceding aren't borked.
	dd mboot		;Make public the mboot header
	dd code			;Make public the code.
	dd bss
	dd end
	dd start

	[GLOBAL start]		;Kernel entry point
	[EXTERN kmain]		;Entry to C code.

start:
	push ebx ;Load location of multiboot header to pass as a parameter to kmain.

	cli			;Disable interrupts
	call kmain

	jmp $			;Enter an infinite loop if kmain ever returns.