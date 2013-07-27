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

lispos-floppy.img: makefile kernel.bin basic-grub-floppy.img grub/menu.lst
	@echo "Creating floppy disk image!"

	$(eval LOOPBACK=$(shell losetup --find))
	$(eval TMP_FLOPPY=$(shell mktemp))
	$(eval FLOPPY_MNT_DIR=$(shell mktemp -d))

	cp basic-grub-floppy.img $(TMP_FLOPPY)

	sudo losetup $(LOOPBACK) $(TMP_FLOPPY)
	sudo mount $(LOOPBACK) $(FLOPPY_MNT_DIR)

	sudo cp kernel.bin $(FLOPPY_MNT_DIR)
	sudo cp grub/menu.lst "$(FLOPPY_MNT_DIR)/grub"
	sync

	sudo umount $(LOOPBACK)
	sudo losetup -d $(LOOPBACK)

	rmdir $(FLOPPY_MNT_DIR)
	mv $(TMP_FLOPPY) $@

.s.o:
	nasm $(ASFLAGS) $<

.PHONY: clean
clean:
	find . -iname *.o | xargs rm
