SOURCES=./arch/x86/boot.o ./arch/x86/io.o ./arch/x86/bootc.o ./arch/x86/interrupt.o ./kernel/main.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -I./arch -I./headers -I./kernel -I./lisp -m32
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf32

all: $(SOURCES) link

clean:
	find . -iname *.o | xargs rm

link:
	ld $(LDFLAGS) -o kernel.bin $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

