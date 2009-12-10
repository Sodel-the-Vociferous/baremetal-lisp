ARCH=x86

KSOURCES=arch/$(ARCH)/boot.o arch/$(ARCH)/io.o arch/$(ARCH)/bootc.o arch/$(ARCH)/interrupt.o io/keymap.o arch/$(ARCH)/kbd.o mem/malloc.o kernel/main.o 
LISPSOURCES=lisp/lisp.o lisp/init.o lisp/lbind.o lisp/read.o lisp/badprint.o
SOURCES=$(KSOURCES) $(LISPSOURCES)

CFLAGS=-nostdlib -nostdinc -fno-builtin -DIN_OS -I./arch/$(ARCH) -I./io -I./headers -I./kernel -I./lisp -I./mem -m32
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf32

all: $(SOURCES) link

clean:
	find . -iname *.o | xargs rm

link:
	ld $(LDFLAGS) -o kernel.bin $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

