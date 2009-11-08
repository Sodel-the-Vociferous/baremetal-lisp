SOURCES=boot/boot.o kernel/main.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -I./boot -I./kernel -I./lisp
LDFLAGS=-Tlink.ld -m elf_i386
ASFLAGS=-felf32

all: $(SOURCES) link

clean:
	-rm *.o ../lib/*.o

link:
	ld $(LDFLAGS) -o kernel.bin $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

