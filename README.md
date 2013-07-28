## My "Bare-Metal" Lisp Interpreter ##

While I was working on it, I generously called this project a Lisp
operating system. The goal was to make it semi-usable, but other
concerns stole my attention away. As it is, it's a very incomplete and
deeply flawed toy lisp interpreter that runs on bare hardware. While
programming the interpreter, I worked off of the Common Lisp
hyper-spec. (That's right: I was gunning for a fully ANSI-compliant
Common Lisp interpreter.)

This project was my second kernel, and my second non-trivial project
in C. It's a bit of a tangly mess.

Compared to the other stuff I've been digging up lately, this is a
comparatively recent project. I stopped working on it after January of
2010 (though, 2010 still feels ancient to me.) I've come a long way
since then, but this program fills me with mellow nostalgia.

### Building and Running It

If you just want to play around with it, you can boot the premade
floppy disk image in qemu with the following command:

    $ qemu-system-i386 -fda src/basic-grub-floppy.img -boot order=a

If you like, you can also write the disk image onto a floppy disk, and
run the "OS" on real hardware:

    $ sudo dd if=src/basic-grub-floppy.img of=/dev/fd0

If you want to use the easy way to build a disk image from the latest
source code, you'll need to have root access via sudo. That way took
the least amount of effort for me to set up.

    $ cd src
    $ make lispos-floppy.img

Then, you can either run qemu manually:

    $ qemu-system-i386 -fda src/lispos-floppy.img -boot order=a

or use the handy shortcut I added to the makefile:

    $ make qemu
