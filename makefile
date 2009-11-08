SOURCES= lisp.c

CFLAGS=-nostdlib -nostdinc -fno-builtin
LDFLAGS=#-Tlink.ld
ASFLAGS=-felf

all: 
	gcc $(CFLAGS) $(SOURCES) -o lisp
	
debug: 
	gcc $(SOURCES) -g -o lisp

clean:
	-rm *.o

link:
	ld $(LDFLAGS) -o lisp $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

