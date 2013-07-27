ARCH=x86
KEYMAP=us

KSOURCES=arch/$(ARCH)/boot.o arch/$(ARCH)/io.o arch/$(ARCH)/bootc.o arch/$(ARCH)/interrupt.o arch/$(ARCH)/screen.o keymap/$(KEYMAP).o arch/$(ARCH)/kbd.o mem/malloc.o kernel/main.o  kernel/terminal.o
LISPSOURCES=lisp/lisp.o lisp/init.o lisp/lbind.o lisp/read.o lisp/badprint.o
SOURCES=$(KSOURCES) $(LISPSOURCES)

CFLAGS=-nostdlib -nostdinc -fno-builtin -g -D IN_OS -D KEYMAP=\"$(KEYMAP).h\" -I./arch/$(ARCH) -I./keymap -I./headers -I./kernel -I./lisp -I./mem -m32
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf32


.PHONY: all
all: kernel.bin lispos-floppy.img

kernel.bin: $(SOURCES)
	ld $(LDFLAGS) -zmuldefs -o $@ $(SOURCES)



.s.o:
	nasm $(ASFLAGS) $<

.PHONY: clean
clean:
	find . -iname *.o | xargs rm
