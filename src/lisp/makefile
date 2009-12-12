SOURCES= init.c lisp.c read.c badprint.c lbind.c
CFLAGS=

all:
	gcc $(SOURCES) $(CFLAGS) -m32 -o lisp.bin

debug:
	gcc $(SOURCES) $(CFLAGS) -m32 -ggdb -o lisp.bin