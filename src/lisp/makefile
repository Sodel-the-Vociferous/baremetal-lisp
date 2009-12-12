SOURCES= init.c lisp.c read.c badprint.c lbind.c
CFLAGS= -m32

all:
	gcc $(SOURCES) $(CFLAGS) -o lisp.bin

debug:
	gcc $(SOURCES) $(CFLAGS) -ggdb -o lisp.bin